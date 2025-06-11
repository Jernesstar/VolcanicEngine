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

#include <Editor/Editor/AssetImporter.h>

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

	GetWindow()->Maximize();

	UIRenderer::Init();

	Application::PushDir();

	ShaderLibrary::Add("Framebuffer",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Framebuffer.glsl.vert",
			"Magma/assets/shaders/Framebuffer.glsl.frag"
		}));
	ShaderLibrary::Add("Lighting",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Lighting.glsl.vert",
			"Magma/assets/shaders/Lighting.glsl.frag"
		}));
	ShaderLibrary::Add("Mesh",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Mesh.glsl.vert",
			"Magma/assets/shaders/Mesh.glsl.frag"
		}));

	ShaderLibrary::Add("Light",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Light.glsl.vert",
			"Magma/assets/shaders/Light.glsl.frag"
		}));

	ShaderLibrary::Add("Bloom-Downsample",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Framebuffer.glsl.vert",
			"Magma/assets/shaders/Downsample.glsl.frag"
		}));
	ShaderLibrary::Add("Bloom-Upsample",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Framebuffer.glsl.vert",
			"Magma/assets/shaders/Upsample.glsl.frag"
		}));
	ShaderLibrary::Add("Bloom",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Framebuffer.glsl.vert",
			"Magma/assets/shaders/Bloom.glsl.frag"
		}));

	ShaderLibrary::Add("Particle-Emit",
		AssetImporter::GetShader({
			"Magma/assets/shaders/ParticleEmitter.glsl.comp",
		}));
	ShaderLibrary::Add("Particle-Update",
		AssetImporter::GetShader({
			"Magma/assets/shaders/ParticleUpdate.glsl.comp",
		}));
	ShaderLibrary::Add("Particle-DefaultDraw",
		AssetImporter::GetShader({
			"Magma/assets/shaders/Particle.glsl.vert",
			"Magma/assets/shaders/Particle.glsl.frag"
		}));

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