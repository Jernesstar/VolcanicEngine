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

#include <Magma/Script/ScriptModule.h>

#include <Magma/UI/UIRenderer.h>

#include <Magma/Scene/Component.h>
#include <Magma/Scene/PhysicsSystem.h>
#include <Magma/Scene/SceneRenderer.h>

#include "Editor/AssetManager.h"
#include "Editor/EditorApp.h"
#include "Editor/Tab.h"

#include "SceneTab.h"
#include "SceneHierarchyPanel.h"

using namespace Magma::ECS;
using namespace Magma::Script;
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

static std::string SelectScriptClass(Ref<ScriptModule> mod);

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

				if(options.add.asset.Type == AssetType::Mesh)
					ImGui::OpenPopup("Create Entity with MeshComponent");
				else if(options.add.asset.Type == AssetType::Audio)
					ImGui::OpenPopup("Create Entity with AudioComponent");
				else if(options.add.asset.Type == AssetType::Script)
					ImGui::OpenPopup("Create Entity with ScriptComponent");
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
		if(options.add.asset.Type == AssetType::Mesh)
			open = ImGui::BeginPopupModal("Create Entity with MeshComponent");
		else if(options.add.asset.Type == AssetType::Audio)
			open = ImGui::BeginPopupModal("Create Entity with AudioComponent");
		else if(options.add.asset.Type == AssetType::Script)
			open = ImGui::BeginPopupModal("Create Entity with ScriptComponent");

		if(open) {
			static std::string str;
			static std::string hint = "Enter entity name";
			ImGui::InputTextWithHint("##Input", hint.c_str(), &str);
			static bool exit = false;
			static Entity newEntity;

			auto& asset = options.add.asset;
			auto& world = m_Context->EntityWorld;
			auto& assetManager = editor.GetAssetManager();

			if(ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			else {
				ImGui::SameLine();
				if(ImGui::Button("Create")
				|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
				{
					exit = true;
					
					if(asset.Type != AssetType::Script) {
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
''
						assetManager.Load(asset);
					}
				}
			}

			if(exit) {
				if(asset.Type == AssetType::Mesh)
					newEntity.Add<MeshComponent>(asset);
				else if(asset.Type == AssetType::Audio)
					newEntity.Add<AudioComponent>(asset);
				else if(asset.Type == AssetType::Script) {
					exit = false;

					auto mod = assetManager.Get<ScriptModule>(asset);
					std::string name = SelectScriptClass(mod);
					if(name != "") {
						exit = true;
						auto _class = mod->GetClass(name);
						auto obj = _class->Instantiate(newEntity);
						newEntity.Add<ScriptComponent>(asset, obj);
					}
				}
			}
			if(exit) {
				asset.Type = AssetType::None;
				ImGui::CloseCurrentPopup();
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

		if(m_Selected && m_Selected.Has<TransformComponent>()) {
			auto view = glm::value_ptr(camera->GetView());
			auto proj = glm::value_ptr(camera->GetProjection());
			auto oper = ImGuizmo::OPERATION::ROTATE;
			auto mode = ImGuizmo::MODE::WORLD;
			auto mat = (glm::mat4)(Transform)m_Selected.Get<TransformComponent>();
			auto ptr = glm::value_ptr(mat);
			// ImGuizmo::SetDrawlist();
			ImGuizmo::Enable(true);
			ImGuizmo::Manipulate(view, proj, oper, mode, ptr);
		}
	}

	ImGui::End();
}

std::string SelectScriptClass(Ref<ScriptModule> mod) {
	static std::string select = "";
	ImGui::OpenPopup("Select Script Class");

	ImGui::BeginPopup("Select Script Class");
	{
		for(const auto& [name, _] : mod->GetClasses()) {
			bool pressed = ImGui::Button(name.c_str());
			if(pressed) {
				select = name;
				ImGui::CloseCurrentPopup();
			}
		}
	}
	ImGui::EndPopup();

	return "";
}

EditorSceneRenderer::EditorSceneRenderer() {
	auto camera = CreateRef<StereographicCamera>(75.0f);
	// camera->SetPosition({ -2.0f, -3.0f, -3.0f });
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	GridPass =
		RenderPass::Create("Grid",
			ShaderPipeline::Create("Magma/assets/shaders", "Grid"), m_Output);
	GridPass->SetData(Renderer3D::GetMeshBuffer());

	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass =
		RenderPass::Create("Lighting",
			ShaderPipeline::Create("Magma/assets/shaders", "Mesh"), m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	// shader = ShaderPipeline::Create("Magma/assets/shaders", "Mask");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	// MaskPass = RenderPass::Create("Mask", shader, buffer);
	// MaskPass->SetData(Renderer3D::GetMeshBuffer());

	// shader = ShaderPipeline::Create("Magma/assets/shaders", "Outline");
	// OutlinePass = RenderPass::Create("Outline", shader, m_Output);
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

void EditorSceneRenderer::Update(TimeStep ts) {
	if(Hovered)
		m_Controller.OnUpdate(ts);
}

void EditorSceneRenderer::Begin() {
	auto camera = m_Controller.GetCamera();
	auto* command = RendererAPI::Get()->NewDrawCommand(GridPass->Get());
	command->Clear = true;
	command->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	command->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());

	auto& call = command->NewDrawCall();
	call.VertexCount = 6;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Single;
	call.DepthTest = DepthTestingMode::Off;
	call.Culling = CullingMode::Off;
	call.Blending = BlendingMode::Greatest;

	FirstCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
	FirstCommand->Clear = false;
	FirstCommand->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	FirstCommand->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void EditorSceneRenderer::SubmitCamera(const Entity& entity) {

}

void EditorSceneRenderer::SubmitSkybox(const Entity& entity) {
	auto& sc = entity.Get<SkyboxComponent>();
	auto& assetManager =
		Application::As<EditorApp>()->GetEditor().GetAssetManager();
	auto cubemap = assetManager.Get<Cubemap>(sc.CubemapAsset);

	FirstCommand->UniformData
	.SetInput("u_Skybox", CubemapSlot{ cubemap });
}

void EditorSceneRenderer::SubmitLight(const Entity& entity) {
	// if(entity.Has<DirectionalLightComponent>()) {
	// 	auto& dc = entity.Get<DirectionalLightComponent>();
	// 	DirectionalLightBuffer->SetData(&dc);
	// 	HasDirectionalLight = true;
	// }
	// else if(entity.Has<PointLightComponent>()) {
	// 	auto& pc = entity.Get<PointLightComponent>();
	// 	PointLightBuffer->SetData(&pc, 1, PointLightCount++);
	// }
	// else if(entity.Has<SpotlightComponent>()) {
	// 	auto& sc = entity.Get<SpotlightComponent>();
	// 	SpotlightBuffer->SetData(&sc, 1, SpotlightCount++);
	// }
}

void EditorSceneRenderer::SubmitParticles(const Entity& entity) {

}

void EditorSceneRenderer::SubmitMesh(const Entity& entity) {
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
	// FirstCommand->UniformData
	// .SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	// FirstCommand->UniformData
	// .SetInput("u_PointLightCount", (int32_t)PointLightCount);
	// FirstCommand->UniformData
	// .SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	// if(Selected) {
	if(false) {
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

	Objects.clear();
	Renderer3D::End();

	Renderer::Flush();

	PointLightCount = 0;
	SpotlightCount = 0;
	HasDirectionalLight = false;
}

}