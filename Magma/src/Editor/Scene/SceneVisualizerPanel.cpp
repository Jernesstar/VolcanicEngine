#include "SceneVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>

#include <Magma/Scene/SceneRenderer.h>
#include <Magma/UI/UIRenderer.h>

#include <Magma/Scene/Component.h>
#include <Magma/Scene/PhysicsSystem.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"

#include "SceneHierarchyPanel.h"

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

static bool s_Hovered = false;

void SceneVisualizerPanel::Update(TimeStep ts) {
	m_Context->OnUpdate(ts);

	if(s_Hovered)
		m_Renderer.Update(ts);
}

struct {
	struct {
		std::string mesh;
	} add;
} static options;

void SceneVisualizerPanel::Draw() {
	m_Context->OnRender(m_Renderer);

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Begin("Scene Visualizer", &m_Open, flags);
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		s_Hovered = ImGui::IsWindowHovered()
				 || Input::GetCursorMode() == CursorMode::Locked;

		auto size = ImGui::GetWindowContentRegionMin();
		auto offset = ImGui::GetWindowPos();
		auto width = size.x;
		auto height = size.y;

		m_Image->Render();

		auto& world = m_Context->EntityWorld;

		if(ImGui::BeginDragDropTarget())
		{
			auto flags = ImGuiDragDropFlags_AcceptBeforeDelivery;
			if(auto payload = ImGui::AcceptDragDropPayload("Image", flags)) {
				if(!payload->IsDelivery()) {
					ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
					ImGui::SetTooltip("Can't drop here yet!");
				}
				else {
					VOLCANICORE_LOG_INFO("Delivered");
				}
			}
			if(auto payload = ImGui::AcceptDragDropPayload("Model", flags)) {
				if(payload->IsDelivery()) {
					ImGui::OpenPopup("Create Entity with MeshComponent");
					options.add.mesh = std::string((const char*)payload->Data);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if(ImGui::BeginPopupModal("Create Entity with MeshComponent")) {
			static std::string str;
			static std::string hint = "Enter entity name";
			ImGui::InputTextWithHint("##Input", hint.c_str(), &str);

			if(ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			else {
				ImGui::SameLine();
				if(ImGui::Button("Create")
				|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
				{
					Entity newEntity;
					bool exit = true;
					if(str != "") {
						if(world.GetEntity(str).IsValid()) {
							exit = false;
							str = "";
							hint = "Entity name must be unique";
						}
						else
							newEntity = world.AddEntity(str);
					}
					else
						newEntity = world.AddEntity();

					if(exit) {
						auto& editor = Application::As<EditorApp>()->GetEditor();
						auto path = editor.GetProject().Path;

						Application::PushDir(path);
						newEntity.Add<MeshComponent>(options.add.mesh);
						Application::PopDir();
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndPopup();
		}

		if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
			auto& cameraController = m_Renderer.GetCameraController();
			// auto world = m_Context->EntityWorld.Get<PhysicsSystem>()->Get();
			auto camera = cameraController.GetCamera();

			glm::vec2 pos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
			glm::vec4 originNDC
			{
				(pos.x/width - 0.5f) * 2.0f,
				(pos.y/height - 0.5f) * 2.0f,
				-1.0f, 1.0f
			};
			glm::vec4 endNDC
			{
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
			// if(hitInfo) {
				VOLCANICORE_LOG_INFO("Hit something");
				// m_Selected.Collider = Ref<RigidBody>(hitInfo.Actor);
				// m_Renderer.Select(entity);
				// auto hierarchy =
				// 	m_Tab->GetPanel("SceneHierarchy")->As<SceneHierarchyPanel>();
				// // hierarchy->Select(entity);
			// }

		}
	}
	ImGui::End();
}

SceneVisualizerPanel::Renderer::Renderer() {
	auto camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

	auto window = Application::GetWindow();

	// Ref<ShaderPipeline> shader;
	// Ref<Framebuffer> frame;
	// shader = ShaderPipeline::Create("Magma/assets/shaders", "Lighting");
	// frame = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	// Lighting = RenderPass::Create("Lighting", shader);
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());
}

void SceneVisualizerPanel::Renderer::Update(TimeStep ts) {

}

void SceneVisualizerPanel::Renderer::SubmitMesh(Entity entity) {
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();
	Transform tr
	{
		.Translation = tc.Translation,
		.Rotation	 = tc.Rotation,
		.Scale		 = tc.Scale
	};

	// auto* command = RendererAPI::Get()->NewDrawCommand(m_DrawPass->Get());
	// Renderer3D::DrawModel(mc.Mesh, tr, command);
}

void SceneVisualizerPanel::Renderer::SubmitLight(Entity entity) {

}

void SceneVisualizerPanel::Renderer::SubmitCamera(Entity entity) {

}

void SceneVisualizerPanel::Renderer::SubmitSkybox(Entity entity) {

}

void SceneVisualizerPanel::Renderer::Render() {

}

}