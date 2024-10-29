#include "SceneVisualizerPanel.h"

#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include "ECS/Component.h"

using namespace Magma::ECS;

namespace Magma {

SceneVisualizerPanel::SceneVisualizerPanel()
	: Panel("SceneVisualizer")
{
	m_Image = CreateRef<UI::Image>(Texture::Create(1920, 1080));
}

SceneVisualizerPanel::SceneVisualizerPanel(Ref<Scene> context)
	: Panel("SceneVisualizer")
{
	m_Image = CreateRef<UI::Image>(Texture::Create(1920, 1080));

	SetContext(context);
}

void SceneVisualizerPanel::SetContext(Ref<Scene> context) {
	m_Context = context;
	auto framebuffer = m_Context->GetRenderer().GetOutput();
	m_Image->SetImage(framebuffer, AttachmentTarget::Color);
	// m_Image->SetImage("Sandbox/assets/images/stone.png");

	m_Selected.Handle = Entity{ };
	// m_Selected.Collider = CreateRef<Physics::RigidBody>();
}

void SceneVisualizerPanel::Draw() {
	m_Context->OnRender();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene Visualizer", &m_Open);
	{
		auto size = ImGui::GetWindowContentRegionMin();
		auto offset = ImGui::GetWindowPos();
		auto width = size.x;
		auto height = size.y;

		m_Image->Render();

		auto& renderer = m_Context->GetRenderer();
		auto& cameraController = renderer.GetCameraController();

		if(ImGui::IsMouseDown(0))
			cameraController.OnUpdate(0.001f);

		if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
			auto& world = m_Context->PhysicsWorld;
			auto camera = cameraController.GetCamera();

			glm::vec2 pos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
			glm::vec4 originNDC{
				(pos.x/width - 0.5f) * 2.0f,
				(pos.y/height - 0.5f) * 2.0f,
				-1.0f, 1.0f
			};
			glm::vec4 endNDC{
				(pos.x/width - 0.5f) * 2.0f,
				(pos.y/height - 0.5f) * 2.0f,
				1.0f, 1.0f
			};

			glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
			glm::vec4 worldStart = invViewProj * originNDC;
			glm::vec4 worldEnd   = invViewProj * endNDC;
			worldStart /= worldStart.w;
			worldEnd   /= worldEnd.w;
			glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
			float maxDist = 10000.0f;

			// auto hitInfo = world.Raycast(worldStart, rayDir, maxDist);
			// m_Selected.Collider = hitInfo.Actor;
		}
	}
	ImGui::PopStyleVar();
	ImGui::End();
}

}