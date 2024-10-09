#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static void FlushCommand(DrawCommand& command);
static void FlushDrawCalls(DrawCommand& command);
static DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map);

const uint64_t Renderer::MaxTriangles = 1'000'000;
const uint64_t Renderer::MaxIndices   = MaxVertices  * 3; /* (3.0f / 2.0f) */
const uint64_t Renderer::MaxVertices  = MaxTriangles * 3;
const uint64_t Renderer::MaxInstances = 200'000;

static Ref<RenderPass> s_RenderPass;
static DrawCommand* s_DrawCommand;
static FrameData s_Frame;

static Buffer<uint32_t> IndexBuffer;
static Buffer<Vertex> GeometryBuffer;
static Buffer<glm::mat4> TransformBuffer;

static uint64_t GeometryIndex;
static uint64_t IndicesIndex;

static uint64_t DrawCallCount;
static uint64_t IndexCount;
static uint64_t VertexCount;
static uint64_t TransformCount;

void DrawCommand::AddPoint(Ref<Point> point, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Point].Partition != DrawPartition::Instanced)
		return;

	if(Points.count(point) == 0)
		Points[point] = TransformBuffer.Partition(400);

	Points[point].Add(transform);
}

void DrawCommand::AddLine(Ref<Line> line, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Line].Partition != DrawPartition::Instanced)
		return;

	if(Lines.count(line) == 0)
		Lines[line] = TransformBuffer.Partition(400);

	Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	if(OptionsMap[DrawPrimitive::Mesh].Partition != DrawPartition::Instanced)
		return;

	// First mesh of its kind in the DrawCommand
	if(Meshes.count(mesh) == 0)
		Meshes[mesh] = TransformBuffer.Partition(400);

	if(Meshes[mesh].GetCount() >= 400) {
		return; // TODO(Implement): What do we do now?
	}

	Meshes[mesh].Add(transform);
	TransformCount++;
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
	VertexCount    = 0;
	TransformCount = 0;

	GeometryIndex = 0;
	IndicesIndex  = 0;
}

void Renderer::EndFrame() {
	Flush();

	s_Frame.Info.DrawCalls = DrawCallCount;
	s_Frame.Info.Indices   = IndexCount;
	s_Frame.Info.Vertices  = VertexCount;
	s_Frame.Info.Instances = TransformCount;

	IndexBuffer.Clear();
	GeometryBuffer.Clear();
	TransformBuffer.Clear();

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
	s_Frame.DrawCommands.push_back(newCommand);

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
	VOLCANICORE_ASSERT(s_DrawCommand);
	s_DrawCommand->Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	VOLCANICORE_ASSERT(s_DrawCommand);
	s_DrawCommand->Size = { width, height };
}

void Renderer::PushOptions(const RendererAPI::Options& options) {
	VOLCANICORE_ASSERT(s_DrawCommand);
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
	auto framebuffer = command.Pass->GetOutput();
	if(!s_DrawCommand || s_DrawCommand->Pass != command.Pass)
		command.Pass->SetGlobalUniforms();

	if(framebuffer) {
		RendererAPI::Get()
			->Resize(framebuffer->GetWidth(), framebuffer->GetHeight());
		framebuffer->Bind();
	}

	// auto oldOptions = RendererAPI::Get()->GetOptions();
	// RendererAPI::Get()->SetOptions(command.RendererOptions);

	command.Pass->SetUniforms(command.GetUniforms());

	if(command.Size != glm::ivec2{ 0, 0 })
		RendererAPI::Get()->Resize(command.Size.x, command.Size.y);
	if(command.Clear)
		RendererAPI::Get()->Clear();

	FlushDrawCalls(command);

	// RendererAPI::Get()->SetOptions(oldOptions);

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		RendererAPI::Get()->Resize(window->GetWidth(), window->GetHeight());
	}

	s_DrawCommand = &command;
}

void FlushDrawCalls(DrawCommand& command) {
	if(command.Points.size() == 0
	&& command.Lines.size()  == 0
	&& command.Meshes.size() == 0) return;

	auto& options = command.OptionsMap;
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

		return;
	}

	for(auto& [mesh, transforms] : command.Meshes) {
		DrawCall meshCall{
			.Type	   = options[DrawPrimitive::Mesh].Type,
			.Partition = options[DrawPrimitive::Mesh].Partition,
			.Primitive = DrawPrimitive::Mesh
		};

		auto& verts = mesh->GetVertices();
		meshCall.GeometryBuffer = GeometryBuffer.Partition(verts.size());
		meshCall.GeometryBuffer.Add(verts);

		if(meshCall.Type == DrawType::Indexed) {
			auto& indices = mesh->GetIndices();
			meshCall.IndexBuffer = IndexBuffer.Partition(indices.size());
			meshCall.IndexBuffer.Add(indices);
		}
		if(meshCall.Partition == DrawPartition::Instanced)
			meshCall.TransformBuffer = std::move(transforms);

		command.Pass->SetUniforms(command.GetUniforms(mesh));
		RendererAPI::Get()->SubmitDrawCall(meshCall);

		uint64_t trCount = meshCall.TransformBuffer.GetCount();
		trCount = trCount ? trCount : 1;

		DrawCallCount++;
		IndexCount  += meshCall.IndexBuffer.GetCount()    * trCount;
		VertexCount += meshCall.GeometryBuffer.GetCount() * trCount;
	}
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