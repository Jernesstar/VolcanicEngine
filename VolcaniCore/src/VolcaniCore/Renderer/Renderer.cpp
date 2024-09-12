#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static Ref<RenderPass> s_CurrentPass;
static DrawCommand s_CurrentDrawCommand;
static FrameData s_CurrentFrame;

void DrawCommand::AddPoint(Ref<Point> point, const glm::mat4& transform) {
	Points[point].Add(transform);
}

void DrawCommand::AddLine(Ref<Line> line, const glm::mat4& transform) {
	Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	Meshes[mesh].Add(transform);
}

void FrameData::AddDrawCommand(DrawCommand command) {
	DrawCommands.push_back(command);
}

void Renderer::Init() {
	s_CurrentFrame = { };
}

void Renderer::Close() {

}

Ref<RenderPass> Renderer::GetPass() {
	return s_CurrentPass;
}
DrawCommand& Renderer::GetDrawCommand() {
	return s_CurrentDrawCommand;
}
FrameDebugInfo& Renderer::GetDebugInfo() {
	return s_CurrentFrame.Info;
}

FrameData& Renderer::GetFrame() {
	return s_CurrentFrame;
}


void Renderer::StartPass(Ref<RenderPass> pass) {
	s_CurrentPass = pass;
}

void Renderer::EndPass() {
	s_CurrentPass = nullptr;
}

void Renderer::Clear(const glm::vec4& color) {
	s_CurrentDrawCommand.Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	s_CurrentDrawCommand.Size = { width, height };
}

static DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map);

void Renderer::NewDrawCommand(const DrawOptionsMap& map) {
	DrawOptionsMap newMap = GetOrReturnDefaults(map);

	auto newCommand = DrawCommand{
		.Pass = s_CurrentPass,
		.OptionsMap = newMap
	};
	s_CurrentFrame.AddDrawCommand(newCommand);
	std::size_t lastIndex = s_CurrentFrame.DrawCommands.size();
	s_CurrentDrawCommand = s_CurrentFrame.DrawCommands.at(lastIndex);
}

void Renderer::BeginFrame() {
	RendererAPI::Get()->StartFrame();
}

void Renderer::EndFrame() {
	s_CurrentFrame.Info.DrawCalls = 0;
	for(auto& command : s_CurrentFrame.DrawCommands) {
		Flush(command);
	}

	s_CurrentFrame.DrawCommands.clear();

	RendererAPI::Get()->EndFrame();
}

void Renderer::Flush(DrawCommand& command) {
	auto framebuffer = command.Pass->GetOutput();
	if(framebuffer) {
		Resize(framebuffer->GetWidth(), framebuffer->GetHeight());
		framebuffer->Bind();
	}

	if(s_CurrentDrawCommand.Pass != command.Pass)
		command.Pass->SetUniforms();

	command.Pass->SetUniforms(command.GetUniforms());

	if(command.Clear)
		Clear();
	if(command.Size != glm::ivec2{ 0, 0 })
		Resize(command.Size.x, command.Size.y);

	auto calls = CreateDrawCalls(command);
	for(auto& call : calls) {
		RendererAPI::Get()->SubmitDrawCall(call);
		s_CurrentFrame.Info.DrawCalls++;
	}

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		Resize(window->GetWidth(), window->GetHeight());
	}

	s_CurrentDrawCommand = command;
}

// TODO(Fix): Reuse draw calls, or have great buffer and partition it.
List<DrawCall> Renderer::CreateDrawCalls(DrawCommand& command) {
	auto& options = command.OptionsMap;
	List<DrawCall> calls;

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

	// 	p0.Position		  = line.P0.Position* transform[0];
	// 	p0.Normal		  = glm::vec3(0.0f);
	// 	p0.TexCoord_Color = line.P0.Color;

	// 	p1.Position		  = line.P1.Position* transform[0];
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
			.Type = DrawType::Indexed,
			.Partition = options[DrawPrimitive::Mesh].Partition,
			.Primitive = DrawPrimitive::Mesh
		};
		meshCall.GeometryBuffer = Buffer<Vertex>(mesh->GetVertices());
		meshCall.IndexBuffer = Buffer<uint32_t>(mesh->GetIndices());

		if(meshCall.Partition == DrawPartition::Instanced) {
			meshCall.TransformBuffer = Buffer<glm::mat4>(transforms);
		}

		calls.push_back(meshCall);
	}

	return calls;
}

static DrawOptionsMap GetOrReturnDefaults(const DrawOptionsMap& map) {
	DrawOptionsMap newMap =
	{
		{
			DrawPrimitive::Point,
			DrawOptions{
				.Type = DrawType::Array,
				.Partition = DrawPartition::Single
			}
		},
		{
			DrawPrimitive::Line,
			DrawOptions{
				.Type = DrawType::Array,
				.Partition = DrawPartition::Single
			}
		},
		{
			DrawPrimitive::Mesh,
			DrawOptions{
				.Type = DrawType::Indexed,
				.Partition = DrawPartition::Instanced
			}
		}
	};

	return newMap;
}

}