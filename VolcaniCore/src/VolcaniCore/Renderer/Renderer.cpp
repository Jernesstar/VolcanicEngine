#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static void FlushCommand(DrawCommand& command);
static List<DrawCall> CreateDrawCalls(DrawCommand& command);
static DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map);

const uint32_t FrameDebugInfo::MaxInstances = 10'000;
const uint32_t FrameDebugInfo::MaxTriangles = 1'000'000;
const uint32_t FrameDebugInfo::MaxVertices  = MaxTriangles * 3;
const uint32_t FrameDebugInfo::MaxIndices   = MaxVertices  * 2; /* (3.0f / 2.0f) */

static Ref<RenderPass> s_RenderPass;
static DrawCommand* s_DrawCommand;
static FrameData s_Frame;

static Buffer<Vertex> GeometryBuffer;
static Buffer<uint32_t> IndexBuffer;
static Buffer<glm::mat4> TransformBuffer;

void DrawCommand::AddPoint(Ref<Point> point, const glm::mat4& transform) {
	Points[point].Add(transform);
}

void DrawCommand::AddLine(Ref<Line> line, const glm::mat4& transform) {
	Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	Meshes[mesh].Add(transform);
}

void FrameData::AddDrawCommand(DrawCommand& command) {
	DrawCommands.push_back(command);
}

void Renderer::Init() {
	s_Frame = { };
	IndexBuffer     = Buffer<uint32_t>(FrameDebugInfo::MaxIndices);
	GeometryBuffer  = Buffer<Vertex>(FrameDebugInfo::MaxVertices);
	// TransformBuffer = Buffer<glm::mat4>(FrameDebugInfo::MaxInstances);
}

void Renderer::Close() {
	GeometryBuffer.Delete();
	IndexBuffer.Delete();
	// TransformBuffer.Delete();
}

void Renderer::BeginFrame() {
	RendererAPI::Get()->StartFrame();

	s_Frame.Info.DrawCalls = 0;
}

void Renderer::EndFrame() {
	Flush();

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
			->Resize(framebuffer->GetWidth(),framebuffer->GetHeight());
		framebuffer->Bind();
	}

	command.Pass->SetGlobalUniforms();
	command.Pass->SetUniforms(command.GetUniforms());

	if(command.Clear)
		RendererAPI::Get()->Clear();
	if(command.Size != glm::ivec2{ 0, 0 })
		RendererAPI::Get()->Resize(command.Size.x, command.Size.y);

	for(auto call : CreateDrawCalls(command)) {
		RendererAPI::Get()->SubmitDrawCall(call);
		s_Frame.Info.DrawCalls++;
	}

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		RendererAPI::Get()->Resize(window->GetWidth(), window->GetHeight());
	}
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

		// meshCall.GeometryBuffer = Buffer<Vertex>(GeometryBuffer, geomIndex++);
		meshCall.GeometryBuffer.Add(mesh->GetVertices());

		if(meshCall.Type == DrawType::Indexed) {
			// meshCall.IndexBuffer = Buffer<uint32_t>(IndexBuffer, indicesIndex++);
			meshCall.IndexBuffer.Add(mesh->GetIndices());
		}

		if(meshCall.Partition == DrawPartition::Instanced)
			meshCall.TransformBuffer = transforms;

		calls.push_back(meshCall);
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