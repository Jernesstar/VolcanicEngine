#include "SceneVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include "ECS/Component.h"

#include "UI/UIRenderer.h"

using namespace Magma::ECS;

namespace Magma {

SceneVisualizerPanel::SceneVisualizerPanel(Scene* context)
	: Panel("SceneVisualizer")
{
	auto width = Application::GetWindow()->GetWidth();
	auto height = Application::GetWindow()->GetHeight();
	m_Image = CreateRef<UI::Image>(Texture::Create(width, height));

	SetContext(context);
}

void SceneVisualizerPanel::SetContext(Scene* context) {
	m_Context = context;
	m_Selected.Handle = Entity{ };
	// m_Selected.Collider = CreateRef<Physics::RigidBody>();

	m_Renderer.SetContext(context);
	m_Image->SetImage(m_Renderer.GetOutput(), AttachmentTarget::Color);
}

void SceneVisualizerPanel::Update(TimeStep ts) {
	m_Renderer.Update(ts);
}

void SceneVisualizerPanel::Draw() {
	m_Context->OnRender(m_Renderer);
	VolcaniCore::Renderer::Flush();

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene Visualizer", &m_Open, flags);
	{
		ImGui::PopStyleVar();

		auto size = ImGui::GetWindowContentRegionMin();
		auto offset = ImGui::GetWindowPos();
		auto width = size.x;
		auto height = size.y;

		m_Image->Render();

		if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
			// auto& cameraController = m_Renderer.GetCameraController();
			// auto& world = m_Context->PhysicsWorld;
			// auto camera = cameraController.GetCamera();

			// glm::vec2 pos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
			// glm::vec4 originNDC
			// {
			// 	(pos.x/width - 0.5f) * 2.0f,
			// 	(pos.y/height - 0.5f) * 2.0f,
			// 	-1.0f, 1.0f
			// };
			// glm::vec4 endNDC
			// {
			// 	(pos.x/width - 0.5f) * 2.0f,
			// 	(pos.y/height - 0.5f) * 2.0f,
			// 	1.0f, 1.0f
			// };

			// glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
			// glm::vec4 worldStart = invViewProj * originNDC;
			// glm::vec4 worldEnd   = invViewProj * endNDC;
			// worldStart /= worldStart.w;
			// worldEnd   /= worldEnd.w;
			// glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
			// float maxDist = 10000.0f;

			// auto hitInfo = world.Raycast(worldStart, rayDir, maxDist);
			// m_Selected.Collider = hitInfo.Actor;
		}
	}
	ImGui::End();
}

void SceneVisualizerPanel::Renderer::Update(TimeStep ts) {

}

void SceneVisualizerPanel::Renderer::Render() {

}

}