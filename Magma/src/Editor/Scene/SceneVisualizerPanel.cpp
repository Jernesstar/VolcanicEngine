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

#include <Lava/App.h>

#include "Editor/AssetManager.h"
#include "Editor/AssetImporter.h"
#include "Editor/EditorApp.h"
#include "Editor/Tab.h"

#include "SceneTab.h"
#include "SceneHierarchyPanel.h"

using namespace Magma::ECS;
using namespace Magma::Script;
using namespace Magma::Physics;
using namespace Lava;

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
			// Add(entity);
		});

	m_Selected = m_Context->EntityWorld.GetEntity("Player");
	m_Renderer.Select(m_Selected);
}

static bool IsLight(ECS::Entity entity) {
	return entity.Has<DirectionalLightComponent>()
		|| entity.Has<PointLightComponent>()
		|| entity.Has<SpotlightComponent>();
}

static glm::vec3 GetLightPosition(ECS::Entity entity) {
	if(entity.Has<DirectionalLightComponent>())
		return entity.Get<DirectionalLightComponent>().Position;
	if(entity.Has<PointLightComponent>())
		return entity.Get<PointLightComponent>().Position;
	if(entity.Has<SpotlightComponent>())
		return entity.Get<SpotlightComponent>().Position;

	return glm::vec3(0.0f);
}

void SceneVisualizerPanel::Add(ECS::Entity entity) {
	if(IsLight(entity)) {
		auto shape = Shape::Create(Shape::Type::Box);
		auto body = RigidBody::Create(RigidBody::Type::Static, shape);
		body->SetTransform({ .Translation = GetLightPosition(entity) });
		body->Data = (void*)(uint64_t)entity.GetHandle();
		m_World.AddActor(body);

		return;
	}

	const auto& tc = entity.Get<TransformComponent>();
	const auto& mc = entity.Get<MeshComponent>();

	// auto shape = Shape::Create(mesh);
	// auto body = RigidBody::Create(RigidBody::Type::Static, shape);
	// body->SetTransform(tc);
	// body->Data = (void*)(uint64_t)entity.GetHandle();
	// m_Selected = entity;
}

void SceneVisualizerPanel::Remove(ECS::Entity entity) {

}

static bool s_Hovered = false;

void SceneVisualizerPanel::Update(TimeStep ts) {
	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto* tab = editor.GetProjectTab()->As<ProjectTab>();
	if(tab->GetState() != ScreenState::Edit)
		return;

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
	auto* tab = editor.GetProjectTab()->As<ProjectTab>();

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse
			   | ImGuiWindowFlags_NoNavInputs;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Begin("Scene Visualizer", &Open, flags);
	{
		ImVec2 pos = ImGui::GetCursorPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		auto width = size.x;
		auto height = size.y;

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
			io.DisplaySize.x, io.DisplaySize.y);

		s_Hovered = ImGui::IsWindowHovered()
			&& !(ImGuizmo::IsOver() || ImGuizmo::IsUsingAny());

		m_Image.SetPosition(pos.x, pos.y);
		m_Image.Draw();
	
		auto camera = m_Renderer.GetCameraController().GetCamera();
		if(m_Selected && m_Selected.Has<TransformComponent>()) {
			const float* view = glm::value_ptr(camera->GetView());
			const float* proj = glm::value_ptr(camera->GetProjection());
			auto oper = ImGuizmo::OPERATION::ROTATE;
			auto mode = ImGuizmo::MODE::WORLD;
			auto& tc = m_Selected.Set<TransformComponent>();
			auto mat = (glm::mat4)(Transform)tc;
			float* ptr = glm::value_ptr(mat);

			ImGuizmo::Enable(true);
			// ImGuizmo::DrawCubes(view, proj, ptr, 1);
			ImGuizmo::Manipulate(view, proj, oper, mode, ptr);
			ImGuizmo::DecomposeMatrixToComponents(ptr,
				&tc.Translation.x, &tc.Rotation.x, &tc.Scale.x);
			tc.Rotation = glm::radians(tc.Rotation);
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		s_Hovered = ImGui::IsWindowHovered()
				&& !(ImGuizmo::IsOver() || ImGuizmo::IsUsingAny());

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

		Ref<Framebuffer> display;
		if(tab->GetState() != ScreenState::Edit) {
			m_Context->OnRender(App::Get()->GetRenderer());
			display = App::Get()->GetRenderer().GetOutput();
		}
		else {
			m_Context->OnRender(m_Renderer);
			display = m_Renderer.GetOutput();
		}

		m_Image.Content = display->Get(AttachmentTarget::Color);

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
				exit = false;
				asset.Type = AssetType::None;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

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
	}

	ImGui::End();
}

std::string SelectScriptClass(Ref<ScriptModule> mod) {
	static std::string select = "";

	ImGui::PushID(mod.get());
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
	ImGui::PopID();

	return "";
}

EditorSceneRenderer::EditorSceneRenderer() {
	Application::PushDir();

	auto camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 1.0f, 15.0f });
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 25.0f;

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	GridPass =
		RenderPass::Create("Grid",
			ShaderPipeline::Create("Magma/assets/shaders", "Grid"), m_Output);
	GridPass->SetData(Renderer3D::GetMeshBuffer());

	BufferLayout instanceLayout =
	{
		{
			{ "Position", BufferDataType::Vec3 }
		},
		true, // Dynamic
		true  // Structure of arrays, aka. Instanced
	};

	DrawBufferSpecification specs
	{
		.VertexLayout = { },
		.InstanceLayout = instanceLayout,
		.MaxIndexCount = 0,
		.MaxVertexCount = 0,
		.MaxInstanceCount = 102
	};
	BillboardBuffer = RendererAPI::Get()->NewDrawBuffer(specs);
	
	BillboardPass =
		RenderPass::Create("Billboard",
			ShaderPipeline::Create("Magma/assets/shaders", "Billboard"),
			m_Output);
	BillboardPass->SetData(BillboardBuffer);

	DirectionalLightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/DirectionalLightIcon.png");
	PointLightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/PointLightIcon.png");
	SpotlightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/SpotlightIcon.png");
	CameraIcon =
		AssetImporter::GetTexture("Magma/assets/icons/CameraIcon.png");

	LightingPass =
		RenderPass::Create("Lighting",
			ShaderPipeline::Create("Magma/assets/shaders", "Lighting"),
			m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	MaskPass =
		RenderPass::Create("Mask",
			ShaderPipeline::Create("Magma/assets/shaders", "Mask"),
			Framebuffer::Create(window->GetWidth(), window->GetHeight()));
	MaskPass->SetData(Renderer3D::GetMeshBuffer());

	OutlinePass =
		RenderPass::Create("Outline",
			ShaderPipeline::Create("Magma/assets/shaders", "Outline"),
			m_Output);
	OutlinePass->SetData(Renderer2D::GetScreenBuffer());

	DirectionalLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Direction", BufferDataType::Vec4 },
			}, 1);
	
	PointLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Constant",  BufferDataType::Float },
				{ "Linear",	   BufferDataType::Float },
				{ "Quadratic", BufferDataType::Float },
			}, 50);

	SpotlightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Direction", BufferDataType::Vec4 },
				{ "CutoffAngle",	  BufferDataType::Float },
				{ "OuterCutoffAngle", BufferDataType::Float },
			}, 50);

	Application::PopDir();
}

EditorSceneRenderer::~EditorSceneRenderer() {
	RendererAPI::Get()->ReleaseBuffer(BillboardBuffer);
}

void EditorSceneRenderer::Update(TimeStep ts) {
	if(Hovered)
		m_Controller.OnUpdate(ts);
}

void EditorSceneRenderer::Begin() {
	auto camera = m_Controller.GetCamera();

	{
		auto* command = RendererAPI::Get()->NewDrawCommand(GridPass->Get());
		command->Clear = true;
		command->DepthTest = DepthTestingMode::Off;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_CameraPosition", camera->GetPosition());

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Single;
	}

	{
		LightingCommand =
			RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
		LightingCommand->Clear = false;
		LightingCommand->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		LightingCommand->UniformData
		.SetInput("u_CameraPosition", camera->GetPosition());
		LightingCommand->UniformData
		.SetInput("u_SceneVisualizer", 1);
	}

	BillboardBuffer->Clear(DrawBufferIndex::Instances);
}

void EditorSceneRenderer::SubmitCamera(const Entity& entity) {
	auto camera = entity.Get<CameraComponent>().Cam;

	if(Selected == entity) {
		// TODO: Frustum lines
	}

	RendererAPI::Get()
	->SetBufferData(BillboardBuffer, DrawBufferIndex::Instances,
					glm::value_ptr(camera->GetPosition()), 1, 101);
}

void EditorSceneRenderer::SubmitSkybox(const Entity& entity) {
	auto& sc = entity.Get<SkyboxComponent>();
	auto* assetManager = App::Get()->GetAssetManager();
	assetManager->Load(sc.CubemapAsset);
	auto cubemap = assetManager->Get<Cubemap>(sc.CubemapAsset);

	LightingCommand->UniformData
	.SetInput("u_Skybox", CubemapSlot{ cubemap });
}

struct DirectionalLight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Direction;

	DirectionalLight(const DirectionalLightComponent& dc)
		: Position(dc.Position, 0.0f), Ambient(dc.Ambient, 0.0f),
		Diffuse(dc.Diffuse, 0.0f), Specular(dc.Specular, 0.0f),
		Direction(dc.Direction, 0.0f) { }
};

struct PointLight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;

	float Constant;
	float Linear;
	float Quadratic;
	// float _padding;

	PointLight(const PointLightComponent& pc)
		: Position(pc.Position, 0.0f), Ambient(pc.Ambient, 0.0f),
		Diffuse(pc.Diffuse, 0.0f), Specular(pc.Specular, 0.0f),
		Constant(pc.Constant), Linear(pc.Linear), Quadratic(pc.Quadratic) { }
};

struct Spotlight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Direction;

	float CutoffAngle;
	float OuterCutoffAngle;
	// float _padding[2];

	Spotlight(const SpotlightComponent& sc)
		: Position(sc.Position, 0.0f), Ambient(sc.Ambient, 0.0f),
		Diffuse(sc.Diffuse, 0.0f), Specular(sc.Specular, 0.0f),
		Direction(sc.Direction, 0.0f),
		CutoffAngle(sc.CutoffAngle), OuterCutoffAngle(sc.OuterCutoffAngle) { }
};

void EditorSceneRenderer::SubmitLight(const Entity& entity) {
	glm::vec3 position;
	uint32_t start;
	uint32_t count;

	if(entity.Has<DirectionalLightComponent>()) {
		DirectionalLight light = entity.Get<DirectionalLightComponent>();
		DirectionalLightBuffer->SetData(&light);
		HasDirectionalLight = true;

		position = light.Position;
		start = 0;
		count = 0;
	}
	else if(entity.Has<PointLightComponent>()) {
		PointLight light = entity.Get<PointLightComponent>();
		PointLightBuffer->SetData(&light, 1, PointLightCount);

		position = light.Position;
		start = 1;
		count = PointLightCount++;
	}
	else if(entity.Has<SpotlightComponent>()) {
		Spotlight light = entity.Get<SpotlightComponent>();
		SpotlightBuffer->SetData(&light, 1, SpotlightCount);

		position = light.Position;
		start = 51;
		count = SpotlightCount++;
	}

	RendererAPI::Get()
	->SetBufferData(BillboardBuffer, DrawBufferIndex::Instances,
					glm::value_ptr(position), 1, start + count);
}

void EditorSceneRenderer::SubmitParticles(const Entity& entity) {

}

void EditorSceneRenderer::SubmitMesh(const Entity& entity) {
	auto* assetManager = App::Get()->GetAssetManager();
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();

	if(!assetManager->IsValid(mc.MeshAsset))
		return;

	assetManager->Load(mc.MeshAsset);
	auto mesh = assetManager->Get<Mesh>(mc.MeshAsset);

	Renderer::StartPass(LightingPass, false);
	{
		Renderer3D::DrawMesh(mesh, tc);
	}
	Renderer::EndPass();
}

void EditorSceneRenderer::Render() {
	LightingCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	LightingCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	LightingCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	LightingCommand->UniformData
	.SetInput(UniformSlot{ DirectionalLightBuffer, "", 0 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ PointLightBuffer, "", 1 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ SpotlightBuffer, "", 2 });

	Renderer3D::End();

	if(Selected) {
		auto* assetManager = App::Get()->GetAssetManager();
		auto& tc = Selected.Get<TransformComponent>();
		auto& mc = Selected.Get<MeshComponent>();
		assetManager->Load(mc.MeshAsset);
		auto mesh = assetManager->Get<Mesh>(mc.MeshAsset);

		{
			auto* command = RendererAPI::Get()->NewDrawCommand(MaskPass->Get());
			command->Clear = true;
			command->UniformData
			.SetInput("u_ViewProj",
				LightingCommand->UniformData.Mat4Uniforms["u_ViewProj"]);
			command->UniformData
			.SetInput("u_Color", glm::vec4(1.0f));

			Renderer3D::DrawMesh(mesh, tc, command);
		}

		Renderer::StartPass(OutlinePass);
		{
			auto width = m_Output->GetWidth();
			auto height = m_Output->GetHeight();

			auto* command = Renderer::GetCommand();
			command->UniformData
			.SetInput("u_PixelSize", 1.0f / glm::vec2(width, height));
			command->UniformData
			.SetInput("u_Color", glm::vec3(0.0f, 0.0f, 1.0f));

			auto mask = MaskPass->GetOutput();
			Renderer2D::DrawFullscreenQuad(mask, AttachmentTarget::Color);
		}
		Renderer::EndPass();
	}

	Renderer3D::End();

	auto camera = m_Controller.GetCamera();
	{
		auto* command =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		command->DepthTest = DepthTestingMode::On;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_BillboardWidth", 1.0f);
		command->UniformData
		.SetInput("u_BillboardHeight", 1.0f);
		command->UniformData
		.SetInput("u_Texture", TextureSlot{ DirectionalLightIcon, 0 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 0;
		call.InstanceCount = (uint64_t)HasDirectionalLight;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
	}

	{
		auto* command =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		command->DepthTest = DepthTestingMode::On;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_BillboardWidth", 1.0f);
		command->UniformData
		.SetInput("u_BillboardHeight", 1.0f);
		command->UniformData
		.SetInput("u_Texture", TextureSlot{ PointLightIcon, 0 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 1;
		call.InstanceCount = PointLightCount;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
	}

	{
		auto* command =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		command->DepthTest = DepthTestingMode::On;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_BillboardWidth", 1.0f);
		command->UniformData
		.SetInput("u_BillboardHeight", 1.0f);
		command->UniformData
		.SetInput("u_Texture", TextureSlot{ SpotlightIcon, 0 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 51;
		call.InstanceCount = SpotlightCount;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
	}

	{
		auto* command =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		command->DepthTest = DepthTestingMode::On;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_BillboardWidth", 1.0f);
		command->UniformData
		.SetInput("u_BillboardHeight", 1.0f);
		command->UniformData
		.SetInput("u_Texture", TextureSlot{ CameraIcon, 0 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 101;
		call.InstanceCount = 1;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
	}

	Renderer::Flush();

	HasDirectionalLight = false;
	PointLightCount = 0;
	SpotlightCount = 0;
}

}