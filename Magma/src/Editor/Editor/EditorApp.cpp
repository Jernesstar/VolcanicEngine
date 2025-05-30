#include "EditorApp.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <ImGuizmo/ImGuizmo.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/ShaderLibrary.h>

#include <Magma/UI/UI.h>

#include <Magma/Physics/Physics.h>

using namespace Magma::UI;
using namespace Magma::Physics;

namespace Magma {

EditorApp::EditorApp(const CommandLineArgs& args)
	: Application(1920, 1080, "Magma Editor")
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UIRenderer::Init();

	Application::PushDir();

	ShaderLibrary::Add("Framebuffer",
		ShaderPipeline::Create("Magma/assets/shaders/", "Framebuffer"));
	ShaderLibrary::Add("Lighting",
		ShaderPipeline::Create("Magma/assets/shaders/", "Lighting"));
	ShaderLibrary::Add("Mesh",
		ShaderPipeline::Create("Magma/assets/shaders/", "Mesh"));
	ShaderLibrary::Add("Particle-Emit",
		ShaderPipeline::Create("Magma/assets/shaders/", "ParticleEmitter"));
	ShaderLibrary::Add("Particle-Update",
		ShaderPipeline::Create("Magma/assets/shaders/", "ParticleUpdate"));
	ShaderLibrary::Add("Particle-DefaultDraw",
		ShaderPipeline::Create("Magma/assets/shaders/", "Particle"));
	// ShaderLibrary::Add("Bloom",
	// 	ShaderPipeline::Create("Magma/assets/shaders/", "Bloom"));

	float fontSize = 15.0f;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(
		"Magma/assets/fonts/JetBrainsMono-Bold.ttf", fontSize);
	io.FontDefault =
		io.Fonts->AddFontFromFileTTF(
			"Magma/assets/fonts/JetBrainsMono-Regular.ttf", fontSize);

	Application::PopDir();

	m_Editor.Load(args);
}

EditorApp::~EditorApp() {
	UIRenderer::Close();
}

void EditorApp::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	UIRenderer::BeginFrame();
	ImGuizmo::BeginFrame();

	m_Editor.Update(ts);
	m_Editor.Render();

	UIRenderer::EndFrame();
}

}