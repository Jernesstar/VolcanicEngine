#include "SceneVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <ImGuizmo/ImGuizmo.h>

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

#include "SceneTab.h"
#include "SceneHierarchyPanel.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

SceneVisualizerPanel::SceneVisualizerPanel(Scene* context)
	: Panel("SceneVisualizer")
{
	m_Image.Width = Application::GetWindow()->GetWidth();
	m_Image.Height = Application::GetWindow()->GetHeight();
	m_Image.Content = m_Renderer.GetOutput()->Get(AttachmentTarget::Color);

	SetContext(context);
}

void SceneVisualizerPanel::SetContext(Scene* context) {
	m_Context = context;
	m_Selected = Entity{ };

	m_Context->EntityWorld
	.ForEach<TransformComponent, MeshComponent>(
		[this](Entity& entity)
		{
			// const auto& tc = entity.Get<TransformComponent>();
			// const auto& mc = entity.Get<MeshComponent>();
			// auto& assetManager =
			// 	Application::As<EditorApp>()->GetEditor().GetAssetManager();
			// auto mesh = assetManager.Get<Mesh>(mc.MeshAsset);

			// auto shape = Shape::Create(mesh);
			// auto body = RigidBody::Create(RigidBody::Type::Static, shape);
			// body->SetTransform(tc);
			// body->Data = (void*)(uint64_t)entity.GetHandle();
			// m_World.AddActor(body);
			// m_Selected = entity;
		});
}

static bool s_Hovered = false;

void SceneVisualizerPanel::Update(TimeStep ts) {
	m_Renderer.IsHovered(s_Hovered);
	m_Renderer.Update(ts);
}

struct {
	struct {
		Asset asset;
	} add;
} static options;

void SceneVisualizerPanel::Draw() {
	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto tab = editor.GetProjectTab()->As<ProjectTab>();

	if(tab->GetState() == ScreenState::Play) {
		// m_Image.Content = tab->GetOutput();
	}
	else
		m_Image.Content = m_Renderer.GetOutput()->Get(AttachmentTarget::Color);

	m_Context->OnRender(m_Renderer);

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Begin("Scene Visualizer", &Open, flags);
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		s_Hovered = ImGui::IsWindowHovered();

		ImVec2 pos = ImGui::GetCursorPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		auto width = size.x;
		auto height = size.y;

		m_Image.SetPosition(pos.x, pos.y);
		// m_Image.SetSize(size.x, size.y);
		m_Image.Draw();

		if(ImGui::BeginDragDropTarget())
		{
			if(auto payload = ImGui::AcceptDragDropPayload("ASSET")) {
				options.add.asset = *(Asset*)payload->Data;

				switch(options.add.asset.Type) {
					case AssetType::Mesh:
						ImGui::OpenPopup("Create Entity with MeshComponent");
						break;
					case AssetType::Cubemap:
						ImGui::OpenPopup("Create Entity with SkyboxComponent");
						break;
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SetCursorPos(pos);

		auto windowFlags = ImGuiWindowFlags_MenuBar;
		auto childFlags = ImGuiChildFlags_Border;
		ImGui::BeginChild("Debug", { 150, 160 }, childFlags, windowFlags);
		{
			auto info = Renderer::GetDebugInfo();
			ImGui::Text("FPS: %0.1f", info.FPS);
			ImGui::Text("Draw Calls: %li", info.DrawCalls);
			ImGui::Text("Indices: %li", info.Indices);
			ImGui::Text("Vertices: %li", info.Vertices);
			ImGui::Text("Instances: %li", info.Instances);
		}
		ImGui::EndChild();

		bool open = false;
		open = ImGui::BeginPopupModal("Create Entity with MeshComponent");
		if(!open)
			open = ImGui::BeginPopupModal("Create Entity with SkyboxComponent");

		if(open) {
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
					auto& world = m_Context->EntityWorld;
					Entity newEntity;
					bool exit = true;
					if(str == "")
						newEntity = world.AddEntity();
					else {
						if(world.GetEntity(str).IsValid()) {
							exit = false;
							str = "";
							hint = "Entity name must be unique";
						}
						else
							newEntity = world.AddEntity(str);
					}

					if(exit) {
						if(options.add.asset.Type == AssetType::Mesh)
							newEntity.Add<MeshComponent>(options.add.asset);
						if(options.add.asset.Type == AssetType::Cubemap)
							newEntity.Add<SkyboxComponent>(options.add.asset);

						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndPopup();
		}

		auto camera = m_Renderer.GetCameraController().GetCamera();
		if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
			// auto& world = m_Context->EntityWorld;

			glm::vec2 pos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
			glm::vec4 originNDC
			{
				(pos.x / width - 0.5f) * 2.0f,
				(pos.y / height - 0.5f) * 2.0f,
				-1.0f, 1.0f
			};
			glm::vec4 endNDC
			{
				(pos.x / width - 0.5f) * 2.0f,
				(pos.y / height - 0.5f) * 2.0f,
				1.0f, 1.0f
			};

			glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
			glm::vec4 worldStart = invViewProj * originNDC;
			glm::vec4 worldEnd   = invViewProj * endNDC;
			worldStart /= worldStart.w;
			worldEnd   /= worldEnd.w;
			glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
			float maxDist = 10000.0f;

			auto hitInfo = m_World.Raycast(worldStart, rayDir, maxDist);
			if(hitInfo)
				m_Selected =
					m_Context->EntityWorld.GetEntity(
						(uint64_t)hitInfo.Actor->Data);
			else
				m_Selected = Entity{ };

			m_Renderer.Select(m_Selected);
			auto hierarchy =
				m_Tab->GetPanel("SceneHierarchy")->As<SceneHierarchyPanel>();
			hierarchy->Select(m_Selected);

			if(hitInfo)
				VOLCANICORE_LOG_INFO("Hit something");
			else
				VOLCANICORE_LOG_INFO("Not hit");
		}

		if(m_Selected) {
			auto view = glm::value_ptr(camera->GetView());
			auto proj = glm::value_ptr(camera->GetProjection());
			auto oper = ImGuizmo::OPERATION::ROTATE;
			auto mode = ImGuizmo::MODE::WORLD;
			auto mat = (glm::mat4)(Transform)m_Selected.Get<TransformComponent>();
			auto ptr = glm::value_ptr(mat);
			ImGuizmo::Enable(true);
			ImGuizmo::Manipulate(view, proj, oper, mode, ptr);
		}
	}
	ImGui::End();
}

EditorSceneRenderer::EditorSceneRenderer() {
	auto camera = CreateRef<StereographicCamera>(75.0f);
	// camera->SetPosition({ -2.0f, -3.0f, -3.0f });
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	Ref<ShaderPipeline> shader;
	Ref<Framebuffer> buffer;

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Grid");
	GridPass = RenderPass::Create("Grid", shader, m_Output);
	GridPass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Mesh");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass = RenderPass::Create("Lighting", shader, m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Mask");
	buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	MaskPass = RenderPass::Create("Mask", shader, buffer);
	MaskPass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Outline");
	OutlinePass = RenderPass::Create("Outline", shader, m_Output);
	OutlinePass->SetData(Renderer2D::GetScreenBuffer());

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

void EditorSceneRenderer::Update(TimeStep ts) {
	if(Hovered)
		m_Controller.OnUpdate(ts);
}

void EditorSceneRenderer::Begin() {
	FirstCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
	FirstCommand->Clear = true;

	FirstCommand->UniformData
	.SetInput("u_ViewProj", m_Controller.GetCamera()->GetViewProjection());
	FirstCommand->UniformData
	.SetInput("u_CameraPosition", m_Controller.GetCamera()->GetPosition());

	auto* command = RendererAPI::Get()->NewDrawCommand(GridPass->Get());
	command->UniformData
	.SetInput("u_ViewProj", m_Controller.GetCamera()->GetViewProjection());
	command->UniformData
	.SetInput("u_CameraPosition", m_Controller.GetCamera()->GetPosition());

	auto& call = command->NewDrawCall();
	call.VertexCount = 6;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Single;
	call.DepthTest = DepthTestingMode::Off;
	call.Culling = CullingMode::Off;
	call.Blending = BlendingMode::Greatest;
}

void EditorSceneRenderer::SubmitCamera(Entity entity) {
	auto camera = entity.Get<CameraComponent>().Cam;
	FirstCommand->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	FirstCommand->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void EditorSceneRenderer::SubmitSkybox(Entity entity) {
	// auto& sc = entity.Get<SkyboxComponent>();

	// FirstCommand->UniformData
	// .SetInput("u_Skybox", CubemapSlot{ sc.Asset.Get<Cubemap>() });
}

void EditorSceneRenderer::SubmitLight(Entity entity) {
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

void EditorSceneRenderer::SubmitMesh(Entity entity) {
	auto& assetManager =
		Application::As<EditorApp>()->GetEditor().GetAssetManager();
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();
	auto mesh = assetManager.Get<Mesh>(mc.MeshAsset);

	if(entity == Selected)
		return;

	if(!Objects.count(mesh))
		Objects[mesh] =
			RendererAPI::Get()->NewDrawCommand(LightingPass->Get());

	auto* command = Objects[mesh];

	Renderer3D::DrawMesh(mesh, tc, command);
}

void EditorSceneRenderer::Render() {
	FirstCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	FirstCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	FirstCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	if(Selected) {
		auto& assetManager =
			Application::As<EditorApp>()->GetEditor().GetAssetManager();
		auto& tc = Selected.Get<TransformComponent>();
		auto& mc = Selected.Get<MeshComponent>();
		auto mesh = assetManager.Get<Mesh>(mc.MeshAsset);

		DrawCommand* command;

		command = RendererAPI::Get()->NewDrawCommand(MaskPass->Get());
		command->Clear = true;
		command->UniformData
		.SetInput("u_ViewProj",
				  FirstCommand->UniformData.Mat4Uniforms["u_ViewProj"]);
		command->UniformData
		.SetInput("u_Color", glm::vec4(1.0f));

		Renderer3D::DrawMesh(mesh, tc, command);

		auto width = (float)Application::GetWindow()->GetWidth();
		auto height = (float)Application::GetWindow()->GetHeight();

		Renderer::StartPass(OutlinePass);
		{
			auto* outlineCommand = Renderer::GetCommand();
			outlineCommand->UniformData
			.SetInput("u_PixelSize", 1.0f/glm::vec2(800, 600));
			outlineCommand->UniformData
			.SetInput("u_Color", glm::vec3(0.0f, 0.0f, 1.0f));

			auto mask = MaskPass->GetOutput();
			Renderer2D::DrawFullscreenQuad(mask, AttachmentTarget::Color);
		}
		Renderer::EndPass();

		command = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
		Renderer3D::DrawMesh(mesh, tc, command);
	}

	Renderer3D::End();
	Objects.clear();

	Renderer::Flush();

	PointLightCount = 0;
	SpotlightCount = 0;
	HasDirectionalLight = false;
}

}