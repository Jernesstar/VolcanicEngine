#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static Ref<RenderPass> s_CurrentPass;
static DrawCommand s_CurrentDrawCommand;
static FrameData s_CurrentFrame;

// template<>
// struct PrimitiveData<Point> { 
// 	std::unordered_map<Ref<Point>, Buffer<glm::mat4>> Points;

// 	Buffer<glm::mat4>& operator [](Ref<Point> points) {
// 		return Points[points];
// 	}
// };
// template<>
// struct PrimitiveData<Line> { 
// 	std::unordered_map<Ref<Line>, Buffer<glm::mat4>> Lines;

// 	Buffer<glm::mat4>& operator [](Ref<Line> line) {
// 		return Lines[line];
// 	}
// };
// template<>
// struct PrimitiveData<Mesh> { 
// 	std::unordered_map<Ref<Mesh>, Buffer<glm::mat4>> Meshes;

// 	Buffer<glm::mat4>& operator [](Ref<Mesh> mesh) {
// 		return Meshes[mesh];
// 	}
// };

void DrawCommand::AddPoint(Ref<Point> point, const glm::mat4& transform) {
	m_Points[point].Add(transform);
}

void DrawCommand::AddLine(Ref<Line> line, const glm::mat4& transform) {
	m_Lines[line].Add(transform);
}

void DrawCommand::AddMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	m_Meshes[mesh].Add(transform);
}

void FrameData::AddDrawCommand(DrawCommand& command) {
	DrawCommands.push_back(command);
}

void Renderer::Init() {

}

void Renderer::Close() {

}

void Renderer::Clear(const glm::vec4& color) {
	s_CurrentDrawCommand.ShouldClearScreen = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	RendererAPI::Get()->Resize(width, height);
	// s_CurrentDrawCommand.Size = { width, height };
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

	if(command.ShouldClearScreen)
		Clear();

	auto calles = CreateDrawCalles(command);
	for(auto& call : calles) {
		RendererAPI::Get()->SubmitDrawCall(call);
		s_CurrentFrame.Info.DrawCalls++;
	}

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		Resize(window->GetWidth(), window->GetHeight());
	}
}

List<DrawCall> Renderer::CreateDrawCalles(DrawCommand& command) {
	// for(auto& [point, transforms] : command.PointTransforms) {

	// }

	// for(auto& [line, transforms] : command.LineTransforms) {
	// 	s_Data.LineArray->Bind();
	// 	s_Data.LineArray->GetVertexBuffer()->SetData(transforms);

	// 	glDrawArraysInstanced(GL_LINES, 0, 2, transforms.GetCount());
	// }

	// Many options here:
	// 1. Put all meshes into s_Data.MeshBuffer and use MultiDrawIndirect
	// 2. Individually bind mesh vertex array and link transform buffer to it
	//		and use DrawInstanced

	// Method 1 currently being used

	// for(auto& [mesh, transforms] : command.MeshTransforms) {
	// 	command.Pass->LinkHandles();

	// 	auto* nativeMesh = mesh->As<OpenGL::Mesh>();
	// 	auto vao = nativeMesh->GetVertexArray();

	// 	vao->Bind();
	// 	s_Data.Transforms->Bind();
	// 	s_Data.Transforms->SetData(transforms);

	// 	DrawInstanced(vao, transforms.GetCount());

	// 	s_Data.Transforms->Unbind();
	// }
}

Ref<RenderPass> Renderer::GetPass() {
	return s_CurrentPass;
}

DrawCommand& Renderer::GetDrawCommand() {
	return s_CurrentDrawCommand;
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	s_CurrentPass = pass;
	s_CurrentDrawCommand = DrawCommand();
	s_CurrentDrawCommand.Pass = s_CurrentPass;
}

void Renderer::EndPass() {
	if(!s_CurrentPass)
		return;

	s_CurrentFrame.AddDrawCommand(s_CurrentDrawCommand);
	s_CurrentPass = nullptr;
}

}