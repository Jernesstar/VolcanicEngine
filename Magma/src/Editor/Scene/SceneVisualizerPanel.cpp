#include "SceneVisualizerPanel.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

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

#include <Magma/UI/UIRenderer.h>
#include <Magma/Scene/Component.h>
#include <Magma/Scene/PhysicsSystem.h>
#include <Magma/Scene/SceneRenderer.h>
#include <Magma/Script/ScriptModule.h>

#include <Lava/Core/App.h>

#include "Editor/AssetManager.h"
#include "Editor/AssetImporter.h"
#include "Editor/EditorApp.h"
#include "Editor/Tab.h"

#include "SceneTab.h"
#include "SceneHierarchyPanel.h"
#include "ComponentEditorPanel.h"

using namespace Magma::ECS;
using namespace Magma::Script;
using namespace Magma::Physics;
using namespace Lava;

namespace Magma {

SceneVisualizerPanel::SceneVisualizerPanel(Scene* context)
	: Panel("SceneVisualizer")
{
	auto& renderer = Editor::GetSceneRenderer();
	m_Image.Content = renderer.GetOutput()->Get(AttachmentTarget::Color);
	m_Image.Width = m_Image.Content->GetWidth();
	m_Image.Height = m_Image.Content->GetHeight();

	Application::PushDir();
	m_Cursor.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/Cursor.png"));
	m_GizmoTranslate.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/GizmoTranslate.png"));
	m_GizmoRotate.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/GizmoRotate.png"));
	m_GizmoScale.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/GizmoScale.png"));
	Application::PopDir();

	m_Cursor.SetSize(35, 35);
	m_GizmoTranslate.SetSize(35, 35);
	m_GizmoRotate.SetSize(35, 35);
	m_GizmoScale.SetSize(35, 35);

	SetContext(context);
}

void SceneVisualizerPanel::SetContext(Scene* context) {
	m_Context = context;
	m_Selected = Entity{ };
	// Editor::GetSceneRenderer().SetContext(context);

	m_Context->EntityWorld
	.ForEach<TransformComponent, MeshComponent>(
		[this](Entity& entity)
		{
			Add(entity);
		});

	m_Context->EntityWorld
	.ForEach<DirectionalLightComponent>(
		[this](Entity& entity)
		{
			Add(entity);
		});

	m_Context->EntityWorld
	.ForEach<PointLightComponent>(
		[this](Entity& entity)
		{
			Add(entity);
		});

	m_Context->EntityWorld
	.ForEach<SpotlightComponent>(
		[this](Entity& entity)
		{
			Add(entity);
		});

	m_Context->EntityWorld
	.ForEach<CameraComponent>(
		[this](Entity& entity)
		{
			Add(entity);
		});
}

void SceneVisualizerPanel::SetImage() {
	Ref<Framebuffer> display = App::Get()->GetRenderer().GetOutput();
	m_Image.Content = display->Get(AttachmentTarget::Color);
}

void SceneVisualizerPanel::ResetImage() {
	auto& renderer = Editor::GetSceneRenderer();
	Ref<Framebuffer> display = renderer.GetOutput();
	m_Image.Content = display->Get(AttachmentTarget::Color);
}

void SceneVisualizerPanel::Add(ECS::Entity entity) {
	glm::vec3 position;
	bool billboard = false;

	if(entity.Has<DirectionalLightComponent>()) {
		position = entity.Get<DirectionalLightComponent>().Position;
		billboard = true;
	}
	if(entity.Has<PointLightComponent>()) {
		position = entity.Get<PointLightComponent>().Position;
		billboard = true;
	}
	if(entity.Has<SpotlightComponent>()) {
		position = entity.Get<SpotlightComponent>().Position;
		billboard = true;
	}
	if(entity.Has<CameraComponent>()) {
		position = entity.Get<CameraComponent>().Cam->GetPosition();
		billboard = true;
	}

	if(billboard) {
		auto shape = Shape::Create(Shape::Type::Box);
		auto body = RigidBody::Create(RigidBody::Type::Static, shape);
		body->SetTransform({ .Translation = position });
		body->Data = (void*)(uint64_t)entity.GetHandle();
		m_World.AddActor(body);

		return;
	}

	const auto& tc = entity.Get<TransformComponent>();
	const auto& mc = entity.Get<MeshComponent>();

	auto* assetManager = AssetManager::Get();
	if(!assetManager->IsValid(mc.MeshSourceAsset))
		return;

	assetManager->Load(mc.MeshSourceAsset);
	auto mesh = assetManager->Get<Mesh>(mc.MeshSourceAsset);

	auto shape = Shape::Create(mesh);
	auto body = RigidBody::Create(RigidBody::Type::Static, shape);
	body->SetTransform(tc);
	body->Data = (void*)(uint64_t)entity.GetHandle();
	m_World.AddActor(body);
}

void SceneVisualizerPanel::Remove(ECS::Entity entity) {

}

static bool s_Hovered = false;

void SceneVisualizerPanel::Update(TimeStep ts) {
	auto* tab = Editor::GetProjectTab();
	if(tab->GetState() != ScreenState::Edit)
		return;

	m_World.OnUpdate(ts);
	auto& renderer = Editor::GetSceneRenderer();
	renderer.IsHovered(s_Hovered);
	renderer.Update(ts);
}

struct {
	struct {
		Asset asset;
	} add;
} static options;

static std::string SelectScriptClass(Ref<ScriptModule> mod);

void SceneVisualizerPanel::Draw() {
	auto* tab = Editor::GetProjectTab();

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse
			   | ImGuiWindowFlags_NoNavInputs;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Begin("Scene Visualizer", &Open, flags);
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

		ImVec2 pos = ImGui::GetCursorPos();
		ImVec2 screenPos = ImGui::GetCursorScreenPos();
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y,
			io.DisplaySize.x, io.DisplaySize.y);

		s_Hovered = ImGui::IsWindowHovered() && !ImGuizmo::IsUsingAny();

		m_Image.Draw();

		auto& renderer = Editor::GetSceneRenderer();
		auto camera = renderer.GetCameraController().GetCamera();
		if(tab->GetState() == ScreenState::Edit && m_Selected && m_GizmoMode) {
			const float* view = glm::value_ptr(camera->GetView());
			const float* proj = glm::value_ptr(camera->GetProjection());
			auto mode = ImGuizmo::MODE::WORLD;
			ImGuizmo::OPERATION oper;
			if(m_GizmoMode == 1)
				oper = ImGuizmo::OPERATION::TRANSLATE;
			else if(m_GizmoMode == 2)
				oper = ImGuizmo::OPERATION::ROTATE;
			else if(m_GizmoMode == 3)
				oper = ImGuizmo::OPERATION::SCALE;

			Transform tc;
			bool canRotate = true;
			bool canScale = true;
			bool enabled = false;

			if(m_Selected.Has<TransformComponent>()) {
				tc = m_Selected.Set<TransformComponent>();
				enabled = true;
			}
			if(m_Selected.Has<DirectionalLightComponent>()) {
				tc.Translation =
					m_Selected.Get<DirectionalLightComponent>().Position;
				tc.Rotation =
					m_Selected.Get<DirectionalLightComponent>().Direction;
				canRotate = true;
				canScale = false;
				enabled = true;
			}
			if(m_Selected.Has<PointLightComponent>()) {
				tc.Translation =
					m_Selected.Get<PointLightComponent>().Position;
				canRotate = false;
				canScale = false;
				enabled = true;
			}
			if(m_Selected.Has<SpotlightComponent>()) {
				tc.Translation =
					m_Selected.Get<SpotlightComponent>().Position;
				tc.Rotation =
					m_Selected.Get<SpotlightComponent>().Direction;
				canRotate = true;
				canScale = false;
				enabled = true;
			}
			if(m_Selected.Has<CameraComponent>()) {
				tc.Translation =
					m_Selected.Get<CameraComponent>().Cam->GetPosition();
				tc.Rotation =
					m_Selected.Get<CameraComponent>().Cam->GetDirection();
				canRotate = true;
				canScale = false;
				enabled = true;
			}

			if(m_GizmoMode == 2 && !canRotate)
				enabled = false;
			if(m_GizmoMode == 3 && !canScale)
				enabled = false;

			ImGuizmo::Enable(enabled);

			if(enabled) {
				auto mat = tc.GetTransform();
				float* ptr = glm::value_ptr(mat);

				ImGuizmo::Manipulate(view, proj, oper, mode, ptr);
				ImGuizmo::DecomposeMatrixToComponents(ptr,
					&tc.Translation.x, &tc.Rotation.x, &tc.Scale.x);
				tc.Rotation = glm::radians(tc.Rotation);

				if(m_Selected.Has<TransformComponent>())
					m_Selected.Set<TransformComponent>() = tc;
				if(m_Selected.Has<DirectionalLightComponent>()) {
					auto& dc = m_Selected.Set<DirectionalLightComponent>();
					dc.Position = tc.Translation;
					dc.Direction = tc.Rotation;
				}
				if(m_Selected.Has<PointLightComponent>()) {
					auto& pc = m_Selected.Set<PointLightComponent>();
					pc.Position = tc.Translation;
				}
				if(m_Selected.Has<SpotlightComponent>()) {
					auto& sc = m_Selected.Set<SpotlightComponent>();
					sc.Position = tc.Translation;
					sc.Direction = tc.Rotation;
				}
				if(m_Selected.Has<CameraComponent>()) {
					auto camera = m_Selected.Set<CameraComponent>().Cam;
					camera->SetPosition(tc.Translation);

					float pitchDelta = tc.Rotation.x;
					float yawDelta   = tc.Rotation.y;
					glm::vec3 up(0.0f, 1.0f, 0.0f);
					glm::vec3 forward = camera->GetDirection();
					glm::vec3 right = glm::normalize(glm::cross(forward, up));

					glm::quat q = glm::cross(glm::angleAxis(-pitchDelta, right),
											glm::angleAxis(-yawDelta, up));
					glm::vec3 finalDir = glm::rotate(glm::normalize(q), forward);
					camera->SetDirection(tc.Rotation);
				}
			}
		}

		s_Hovered = ImGui::IsWindowHovered()
				&& !(ImGuizmo::IsOver() || ImGuizmo::IsUsingAny());

		if(ImGui::BeginDragDropTarget())
		{
			if(auto payload = ImGui::AcceptDragDropPayload("ASSET"))
				options.add.asset = *(Asset*)payload->Data;
			ImGui::EndDragDropTarget();
		}

		if(options.add.asset.Type == AssetType::Mesh)
			ImGui::OpenPopup("Create Entity with MeshComponent");
		else if(options.add.asset.Type == AssetType::Audio)
			ImGui::OpenPopup("Create Entity with AudioComponent");
		else if(options.add.asset.Type == AssetType::Script)
			ImGui::OpenPopup("Create Entity with ScriptComponent");

		ImGui::SetCursorPos(pos);

		auto childFlags = ImGuiChildFlags_Border;
		ImGui::BeginChild("Debug", { 135, 120 }, childFlags, 0);
		{
			auto info = Renderer::GetDebugInfo();
			ImGui::Text("FPS: %0.1f", info.FPS);
			ImGui::Text("Draw Calls: %li", info.DrawCalls);
			ImGui::Text("Indices: %li", info.Indices);
			ImGui::Text("Vertices: %li", info.Vertices);
			ImGui::Text("Instances: %li", info.Instances);
		}
		ImGui::EndChild();

		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::SetCursorPos({ pos.x + size.x - 170, pos.y + 10 });

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 15.0f);
		ImGui::BeginChild("##Buttons", { 165, 45 });
		{
			m_Cursor.SetPosition(5, 5);
			m_GizmoTranslate.SetPosition(45, 5);
			m_GizmoRotate.SetPosition(85, 5);
			m_GizmoScale.SetPosition(125, 5);

			m_Cursor.Render();
			m_GizmoTranslate.Render();
			m_GizmoRotate.Render();
			m_GizmoScale.Render();

			if(m_Cursor.GetState().Clicked)
				m_GizmoMode = 0;
			if(m_GizmoTranslate.GetState().Clicked)
				m_GizmoMode = 1;
			if(m_GizmoRotate.GetState().Clicked)
				m_GizmoMode = 2;
			if(m_GizmoScale.GetState().Clicked)
				m_GizmoMode = 3;
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if(tab->GetState() == ScreenState::Edit)
			m_Context->OnRender(renderer);

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
			static bool tryExit = false;
			static Entity newEntity;

			auto& asset = options.add.asset;
			auto& world = m_Context->EntityWorld;
			auto* assetManager = AssetManager::Get();

			if(ImGui::Button("Cancel")) {
				options.add.asset = { };
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if(ImGui::Button("Create")
			|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
			{
				exit = true;
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
				if(asset.Type == AssetType::Script)
					tryExit = true;

				assetManager->Load(asset);
			}

			if(asset.Type == AssetType::Script && tryExit) {
				exit = false;

				auto mod = assetManager->Get<ScriptModule>(asset);
				std::string name = SelectScriptClass(mod);
				if(name != "") {
					exit = true;
					tryExit = false;

					auto _class = mod->GetClass(name);
					auto obj = _class->Construct();
					newEntity.Add<ScriptComponent>(asset, obj);
				}
			}

			if(exit) {
				if(asset.Type == AssetType::Mesh)
					newEntity.Add<MeshComponent>(
						asset, Asset{ 0, AssetType::Material });
				else if(asset.Type == AssetType::Audio)
					newEntity.Add<AudioComponent>(asset);

				exit = false;
				asset.Type = AssetType::None;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
			glm::vec2 absPos
			{
				ImGui::GetMousePos().x - vMin.x,
				ImGui::GetMousePos().y - vMin.y
			};

			float windowWidth = renderer.GetOutput()->GetWidth();
			float windowHeight = renderer.GetOutput()->GetHeight();
			glm::vec4 originNDC // 0 -> windowSize => -1 -> 1
			{
				(absPos.x / windowWidth - 0.5f) * 2.0f,
				(absPos.y / windowHeight - 0.5f) * 2.0f,
				-1.0f, 1.0f
			};
			glm::vec4 endNDC
			{
				(absPos.x / windowWidth - 0.5f) * 2.0f,
				-(absPos.y / windowHeight - 0.5f) * 2.0f,
				1.0f, 1.0f
			};

			glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
			glm::vec4 worldStart = invViewProj * originNDC;
			glm::vec4 worldEnd   = invViewProj * endNDC;
			worldStart /= worldStart.w;
			worldEnd   /= worldEnd.w;
			glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
			float maxDist = 1'000'000.0f;

			auto hitInfo = m_World.Raycast(worldStart, rayDir, maxDist);
			if(hitInfo)
				m_Selected =
					m_Context->EntityWorld.GetEntity(
						(uint64_t)hitInfo.Actor->Data);
			else
				m_Selected = Entity{ };

			renderer.Select(m_Selected);
			auto hierarchy =
				m_Tab->GetPanel("SceneHierarchy")->As<SceneHierarchyPanel>();
			hierarchy->Select(m_Selected);
		}
	}

	ImGui::End();
}

std::string SelectScriptClass(Ref<ScriptModule> mod) {
	static std::string select = "";

	ImGui::OpenPopup("Select Script Class");
	if(ImGui::BeginPopupModal("Select Script Class"))
	{
		for(const auto& [name, _] : mod->GetClasses()) {
			bool pressed = ImGui::Button(name.c_str());
			if(pressed) {
				select = name;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	if(select != "") {
		std::string val = select;
		select = "";
		return val;
	}

	return "";
}

}