#include "Renderer.h"

#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static Ref<RenderPass> s_CurrentPass;
static DrawCommand s_CurrentDrawCommand;
static FrameData s_CurrentFrame;

void DrawCommand::AddPoint(const Point& point, const glm::mat4& transform) {
	Points[point].Add(transform);
}

void DrawCommand::AddLine(const Line& line, const glm::mat4& transform) {
	Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	Meshes[mesh].Add(transform);
}

void FrameData::AddDrawCommand(DrawCommand& command) {
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

void Renderer::StartPass(Ref<RenderPass> pass, const DrawOptionsMap& map) {
	s_CurrentPass = pass;

	// TODO(Implement): GetValueOrDefault(DrawOptionsMap), similar to the one used in CameraController
	if(map == { }) {
		map =
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
			}
			{
				DrawPrimitive::Mesh,
				DrawOptions{
					.Type = DrawType::Indexed,
					.Partition = DrawPartition::Instanced
				}
			}
		}
	}
	s_CurrentDrawCommand = DrawCommand{
		.Pass = pass,
		.Map = map
	};
}

void Renderer::EndPass() {
	if(!s_CurrentPass)
		return;

	s_CurrentFrame.AddDrawCommand(s_CurrentDrawCommand);
	s_CurrentPass = nullptr;
}

void Renderer::Clear(const glm::vec4& color) {
	s_CurrentDrawCommand.Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	s_CurrentDrawCommand.Size = { width, height };
}

void Renderer::BeginFrame() {
	RendererAPI::Get()->StartFrame();
}

void Renderer::EndFrame() {
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

	if(command.Clear)
		Clear();
	if(command.Size != { 0, 0 })
		Resize(command.Size.x, command.Size.y);

	command.Pass->SetUniforms();

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
}

List<DrawCall> Renderer::CreateDrawCalls(DrawCommand& command) {
	auto& options = command.OptionsMap;
	List<DrawCall> calls;

	// TODO(Implement):
	DrawCall pointCall{
		.Type	   = options[DrawPrimitive::Point].Type,
		.Partition	   = options[DrawPrimitive::Point].Partition,
		.Primitive = DrawPrimitive::Point
	};
	for(auto& [point, transforms] : command.Points) {
		Vertex p0;

		p0.Position = point.Position * transforms[0];
		p0.Normal = glm::vec3(0.0f);
		p0.TexCoord_Color = point.Color;

		pointCall.GeometryBuffer->Add(p0);
	}

	// TODO(Implement):
	DrawCall lineCall{
		.Type	   = options[DrawPrimitive::Line].Type,
		.Partition = options[DrawPrimitive::Line].Partition,
		.Primitive = DrawPrimitive::Line
	};
	for(auto& [line, _] : command.Lines) {
		Vertex p0, p1;

		p0.Position		  = line.P0.Position;
		p0.Normal		  = glm::vec3(0.0f);
		p0.TexCoord_Color = line.P0.Color;

		p1.Position		  = line.P1.Position;
		p1.Normal		  = glm::vec3(0.0f);
		p1.TexCoord_Color = line.P1.Color;

		lineCall.GeometryBuffer->Add(p0);
		lineCall.GeometryBuffer->Add(p1);
	}

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

		meshCall.GeometryBuffer->Add(mesh.GetVertices());
		meshCall.IndexBuffer->Add(mesh.GetIndices());

		if(meshCall.Partition == DrawPartition::Instanced) {
			meshCall.TransformBuffer->Add(transforms);
		}

		calls.push_back(meshCall);
	}

	return calls;
}

}