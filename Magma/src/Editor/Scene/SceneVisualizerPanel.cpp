#include "SceneVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
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

	m_Image->SetImage(m_Renderer.GetOutput()->Get(AttachmentTarget::Color));
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

		s_Hovered = ImGui::IsWindowHovered();

		ImVec2 pos = ImGui::GetCursorPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		auto width = size.x;
		auto height = size.y;

		m_Image->SetPosition(pos.x, pos.y);
		// m_Image->SetSize(size.x, size.y);
		m_Image->Draw();

		// ImGui::SetCursorPos(pos);

		// auto windowFlags = ImGuiWindowFlags_MenuBar;
		// auto childFlags = ImGuiChildFlags_Border;
		// ImGui::BeginChild("Debug", { 200, 200 }, childFlags, windowFlags);
		// {
		// 	auto info = VolcaniCore::Renderer::GetDebugInfo();
		// 	ImGui::Text("FPS: %0.1f", info.FPS);
		// 	ImGui::Text("Draw Calls: %li", info.DrawCalls);
		// 	ImGui::Text("Indices: %li", info.Indices);
		// 	ImGui::Text("Vertices: %li", info.Vertices);
		// 	ImGui::Text("Instances: %li", info.Instances);
		// 	ImGui::Text("Triangles: %li",
		// 		info.Instances * uint64_t(info.Vertices / 3));
		// }
		// ImGui::EndChild();

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

		auto& world = m_Context->EntityWorld;
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
			// auto physicsWorld = world.Get<PhysicsSystem>()->Get();
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

			// auto hitInfo = physicsWorld.Raycast(worldStart, rayDir, maxDist);
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

	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	Ref<ShaderPipeline> shader;
	Ref<Framebuffer> buffer;

	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass = RenderPass::Create("Lighting", shader, m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	// shader = ShaderPipeline::Create("Magma/assets/shaders", "Mask");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	// MaskPass = RenderPass::Create("Mask", shader, buffer);
	// MaskPass->SetData(Renderer3D::GetMeshBuffer());

	// shader = ShaderPipeline::Create("Magma/assets/shaders", "Outline");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	// OutlinePass = RenderPass::Create("Outline", shader, buffer);
	// OutlinePass->SetData(Renderer2D::GetScreenBuffer());

	// DirectionalLightBuffer =
	// 	UniformBuffer::Create(
	// 		BufferLayout
	// 		{
	// 			{ "Ambient",   BufferDataType::Vec3 },
	// 			{ "Diffuse",   BufferDataType::Vec3 },
	// 			{ "Specular",  BufferDataType::Vec3 },
	// 			{ "Direction", BufferDataType::Vec3 },
	// 		});

	// PointLightBuffer =
	// 	UniformBuffer::Create(
	// 		BufferLayout
	// 		{
	// 			{ "Ambient",   BufferDataType::Vec3 },
	// 			{ "Diffuse",   BufferDataType::Vec3 },
	// 			{ "Specular",  BufferDataType::Vec3 },
	// 			{ "Position",  BufferDataType::Vec3 },
	// 			{ "Direction", BufferDataType::Vec3 },
	// 			{ "Constant",  BufferDataType::Float },
	// 			{ "Linear",	   BufferDataType::Float },
	// 			{ "Quadratic", BufferDataType::Float },
	// 		}, 100);

	// SpotlightBuffer =
	// 	UniformBuffer::Create(
	// 		BufferLayout
	// 		{
	// 			{ "Ambient",   BufferDataType::Vec3 },
	// 			{ "Diffuse",   BufferDataType::Vec3 },
	// 			{ "Specular",  BufferDataType::Vec3 },
	// 			{ "Position",  BufferDataType::Vec3 },
	// 			{ "Direction", BufferDataType::Vec3 },
	// 			{ "CutoffAngle",	  BufferDataType::Float },
	// 			{ "OuterCutoffAngle", BufferDataType::Float },
	// 		}, 100);
}

void SceneVisualizerPanel::Renderer::Update(TimeStep ts) {
	m_Controller.OnUpdate(ts);
}

void SceneVisualizerPanel::Renderer::Begin() {
	FirstCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
}

void SceneVisualizerPanel::Renderer::SubmitCamera(Entity entity) {

}

void SceneVisualizerPanel::Renderer::SubmitSkybox(Entity entity) {
	// auto& sc = entity.Get<SkyboxComponent>();

	// FirstCommand->UniformData
	// .SetInput("u_Skybox", CubemapSlot{ sc.Asset.Get<Cubemap>() });
}

void SceneVisualizerPanel::Renderer::SubmitLight(Entity entity) {
	if(entity.Has<DirectionalLightComponent>()) {
		auto& dc = entity.Get<DirectionalLightComponent>();
		DirectionalLightBuffer->SetData(&dc);
		HasDirectionalLight = true;
	}
	else if(entity.Has<PointLightComponent>()) {
		auto& pc = entity.Get<PointLightComponent>();
		PointLightBuffer->SetData(&pc, 1, PointLightCount++);
	}
	else if(entity.Has<SpotlightComponent>()) {
		auto& sc = entity.Get<SpotlightComponent>();
		SpotlightBuffer->SetData(&sc, 1, SpotlightCount++);
	}
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

	if(!Objects.count(mc.Mesh)) {
		// if(entity == Selected) {
		// 	auto* command = Objects[mc.Mesh] =
		// 		RendererAPI::Get()->NewDrawCommand(MaskPass->Get());
		// 	command->UniformData
		// 	.SetInput("u_Color", glm::vec4(1.0f));
		// }
		// else {
			auto* command = Objects[mc.Mesh] =
				RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
		// }
	}
	auto* command = Objects[mc.Mesh];

	Renderer3D::DrawModel(mc.Mesh, tr, command);
}

void SceneVisualizerPanel::Renderer::Render() {
	FirstCommand->Clear = true;
	FirstCommand->UniformData
	.SetInput("u_ViewProj", m_Controller.GetCamera()->GetViewProjection());
	FirstCommand->UniformData
	.SetInput("u_CameraPosition", m_Controller.GetCamera()->GetPosition());
	FirstCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	FirstCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	FirstCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	VolcaniCore::Renderer::Flush();

	Renderer3D::End();
	Objects.clear();

	PointLightCount = 0;
	SpotlightCount = 0;
	HasDirectionalLight = false;
}

}