#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static void FlushCommand(DrawCommand& command);
static List<DrawCall> CreateDrawCalls(DrawCommand& command);
static DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map);

const uint32_t Renderer::MaxTriangles = 1'000'000;
const uint32_t Renderer::MaxIndices   = MaxVertices  * 3; /* (3.0f / 2.0f) */
const uint32_t Renderer::MaxVertices  = MaxTriangles * 3;
const uint32_t Renderer::MaxInstances = 10'000;

static Ref<RenderPass> s_RenderPass;
static DrawCommand* s_DrawCommand;
static FrameData s_Frame;

static Buffer<uint32_t> IndexBuffer;
static Buffer<Vertex> GeometryBuffer;
static Buffer<glm::mat4> TransformBuffer;

static uint32_t DrawCallCount;
static uint32_t IndexCount;
static uint32_t GeometryCount;
static uint32_t TransformCount;

void DrawCommand::AddPoint(Ref<Point> point, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Point].Partition != DrawPartition::Instanced) {
		Points[point] = Buffer<glm::mat4>();
		return;
	}

	if(Points.count(point) == 0)
		Points[point] = Buffer<glm::mat4>(TransformBuffer, TransformCount);

	Points[point].Add(transform);
}

void DrawCommand::AddLine(Ref<Line> line, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Line].Partition != DrawPartition::Instanced) {
		Lines[line] = Buffer<glm::mat4>();
		return;
	}

	if(Lines.count(line) == 0)
		Lines[line] = Buffer<glm::mat4>(TransformBuffer, TransformCount);

	Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Mesh].Partition != DrawPartition::Instanced) {
		Meshes[mesh] = Buffer<glm::mat4>();
		return;
	}

	// TODO(Fix): Support multiple meshes
	if(Meshes.count(mesh) == 0)
		Meshes[mesh] = Buffer<glm::mat4>(TransformBuffer, TransformCount);

	Meshes[mesh].Add(transform);
}

void FrameData::AddDrawCommand(DrawCommand& command) {
	DrawCommands.push_back(command);
}

void Renderer::Init() {
	s_Frame = { };
	IndexBuffer     = Buffer<uint32_t>(Renderer::MaxIndices);
	GeometryBuffer  = Buffer<Vertex>(Renderer::MaxVertices);
	TransformBuffer = Buffer<glm::mat4>(Renderer::MaxInstances);
}

void Renderer::Close() {
	IndexBuffer.Delete();
	GeometryBuffer.Delete();
	TransformBuffer.Delete();
}

void Renderer::BeginFrame() {
	RendererAPI::Get()->StartFrame();

	DrawCallCount  = 0;
	IndexCount     = 0;
	GeometryCount  = 0;
	TransformCount = 0;
}

void Renderer::EndFrame() {
	Flush();

	s_Frame.Info.DrawCalls = DrawCallCount;
	s_Frame.Info.Indices   = IndexCount;
	s_Frame.Info.Vertices  = GeometryCount;
	s_Frame.Info.Instances = TransformCount;

	IndexBuffer.Clear();
	GeometryBuffer.Clear();
	TransformBuffer.Clear();
	TransformCount = 0;

	RendererAPI::Get()->EndFrame();
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	s_RenderPass = pass;

	NewDrawCommand();
}

void Renderer::EndPass() {
	s_RenderPass = nullptr;
	s_DrawCommand = nullptr;
}

void Renderer::NewDrawCommand(const DrawOptionsMap& map) {
	DrawOptionsMap newMap = GetOrReturnDefaults(map);

	auto newCommand = DrawCommand{
		.Pass = s_RenderPass,
		.OptionsMap = newMap,
	};
	s_Frame.AddDrawCommand(newCommand);

	std::size_t lastIndex = s_Frame.DrawCommands.size() - 1;
	s_DrawCommand = &s_Frame.DrawCommands[lastIndex];
}

void Renderer::Flush() {
	for(auto& command : s_Frame.DrawCommands) {
		FlushCommand(command);
	}

	s_Frame.DrawCommands.clear();
}

void Renderer::Clear(const glm::vec4& color) {
	s_DrawCommand->Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	s_DrawCommand->Size = { width, height };
}

void Renderer::PushOptions(const RendererAPI::Options& options) {
	s_DrawCommand->RendererOptions = options;
}

Ref<RenderPass> Renderer::GetPass() {
	return s_RenderPass;
}

DrawCommand& Renderer::GetDrawCommand() {
	return *s_DrawCommand;
}

FrameDebugInfo Renderer::GetDebugInfo() {
	return s_Frame.Info;
}

FrameData& Renderer::GetFrame() { return s_Frame; }

void FlushCommand(DrawCommand& command) {
	if(command.Points.size() == 0
	&& command.Lines.size()  == 0
	&& command.Meshes.size() == 0) return;

	auto framebuffer = command.Pass->GetOutput();
	if(framebuffer) {
		RendererAPI::Get()
			->Resize(framebuffer->GetWidth(), framebuffer->GetHeight());
		framebuffer->Bind();
	}

	auto oldOptions = RendererAPI::Get()->GetOptions();
	RendererAPI::Get()->SetOptions(command.RendererOptions);

	if(!s_DrawCommand || s_DrawCommand->Pass != command.Pass)
		command.Pass->SetGlobalUniforms();

	command.Pass->SetUniforms(command.GetUniforms());

	if(command.Clear)
		RendererAPI::Get()->Clear();
	if(command.Size != glm::ivec2{ 0, 0 })
		RendererAPI::Get()->Resize(command.Size.x, command.Size.y);

	// TODO(Fix): ProcessDrawCalls: Flush draw calls if data becomes to big
	for(auto& call : CreateDrawCalls(command)) {
		RendererAPI::Get()->SubmitDrawCall(call);
	}

	RendererAPI::Get()->SetOptions(oldOptions);

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		RendererAPI::Get()->Resize(window->GetWidth(), window->GetHeight());
	}

	s_DrawCommand = &command;
}

List<DrawCall> CreateDrawCalls(DrawCommand& command) {
	auto& options = command.OptionsMap;
	List<DrawCall> calls;
	uint32_t geomIndex = 0, indicesIndex = 0;

	// // TODO(Implement):
	// DrawCall pointCall{
	// 	.Type	   = options[DrawPrimitive::Point].Type,
	// 	.Partition = options[DrawPrimitive::Point].Partition,
	// 	.Primitive = DrawPrimitive::Point
	// };
	// for(auto& [point, transform] : command.Points) {
	// 	Vertex p0;

	// 	p0.Position = point->Position * transform[0];
	// 	p0.Normal = glm::vec3(0.0f);
	// 	p0.TexCoord_Color = point->Color;

	// 	pointCall.GeometryBuffer->Add(p0);
	// }

	// // TODO(Implement):
	// DrawCall lineCall{
	// 	.Type	   = options[DrawPrimitive::Line].Type,
	// 	.Partition = options[DrawPrimitive::Line].Partition,
	// 	.Primitive = DrawPrimitive::Line
	// };
	// for(auto& [line, transform] : command.Lines) {
	// 	Vertex p0, p1;

	// 	p0.Position		  = line.P0.Position * transform[0];
	// 	p0.Normal		  = glm::vec3(0.0f);
	// 	p0.TexCoord_Color = line.P0.Color;

	// 	p1.Position		  = line.P1.Position * transform[0];
	// 	p1.Normal		  = glm::vec3(0.0f);
	// 	p1.TexCoord_Color = line.P1.Color;

	// 	lineCall.GeometryBuffer->Add(p0);
	// 	lineCall.GeometryBuffer->Add(p1);
	// }

	if(options[DrawPrimitive::Mesh].Partition == DrawPartition::Multi) {
		// TODO(Implement):

		return calls;
	}

	for(auto& [mesh, transforms] : command.Meshes) {
		DrawCall meshCall{
			.Type	   = options[DrawPrimitive::Mesh].Type,
			.Partition = options[DrawPrimitive::Mesh].Partition,
			.Primitive = DrawPrimitive::Mesh
		};

		meshCall.GeometryBuffer = Buffer<Vertex>(GeometryBuffer, geomIndex);
		meshCall.GeometryBuffer.Add(mesh->GetVertices());

		if(meshCall.Type == DrawType::Indexed) {
			meshCall.IndexBuffer = Buffer<uint32_t>(IndexBuffer, indicesIndex);
			meshCall.IndexBuffer.Add(mesh->GetIndices());
		}
		if(meshCall.Partition == DrawPartition::Instanced)
			meshCall.TransformBuffer = transforms;

		calls.push_back(meshCall);
		geomIndex += meshCall.GeometryBuffer.GetCount();
		indicesIndex += meshCall.IndexBuffer.GetCount();

		uint32_t multiple = meshCall.TransformBuffer.GetCount();
		multiple = multiple != 0 ? multiple : 1;

		DrawCallCount++;
		IndexCount     += meshCall.IndexBuffer.GetCount() * multiple;
		GeometryCount  += meshCall.GeometryBuffer.GetCount() * multiple;
		TransformCount += multiple;
	}

	return calls;
}

DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map) {
	DrawOptionsMap newMap =
	{
		{
			DrawPrimitive::Point,
			DrawOptions
			{
				.Type = DrawType::Array,
				.Partition = DrawPartition::Single
			}
		},
		{
			DrawPrimitive::Line,
			DrawOptions
			{
				.Type = DrawType::Array,
				.Partition = DrawPartition::Single
			}
		},
		{
			DrawPrimitive::Mesh,
			DrawOptions
			{
				.Type = DrawType::Indexed,
				.Partition = DrawPartition::Instanced
			}
		}
	};

	return newMap;
}

}