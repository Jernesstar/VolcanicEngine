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

	Ref<Framebuffer> display;
	if(tab->GetState() == ScreenState::Play)
		display = App::Get()->GetRenderer().GetOutput();
	else {
		m_Context->OnRender(m_Renderer);
		display = m_Renderer.GetOutput();
	}

	m_Image.Content = display->Get(AttachmentTarget::Color);

	auto flags = ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse
			   | ImGuiWindowFlags_NoNavInputs;
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
	auto camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

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
		.MaxInstanceCount = 101
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

	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass =
		RenderPass::Create("Lighting",
			ShaderPipeline::Create("Magma/assets/shaders", "Mesh"), m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	MaskPass =
		RenderPass::Create("Mask",
			ShaderPipeline::Create("Magma/assets/shaders", "Mask"),
			Framebuffer::Create(window->GetWidth(), window->GetHeight()));
	MaskPass->SetData(Renderer3D::GetMeshBuffer());

	OutlinePass =
		RenderPass::Create("Outline",
			ShaderPipeline::Create("Magma/assets/shaders", "Outline"), m_Output);
	OutlinePass->SetData(Renderer2D::GetScreenBuffer());

	DirectionalLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Ambient",   BufferDataType::Vec3 },
				{ "Diffuse",   BufferDataType::Vec3 },
				{ "Specular",  BufferDataType::Vec3 },
				{ "Direction", BufferDataType::Vec3 },
			}, 1);

	PointLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Ambient",   BufferDataType::Vec3 },
				{ "Diffuse",   BufferDataType::Vec3 },
				{ "Specular",  BufferDataType::Vec3 },
				{ "Position",  BufferDataType::Vec3 },
				{ "Direction", BufferDataType::Vec3 },
				{ "Constant",  BufferDataType::Float },
				{ "Linear",	   BufferDataType::Float },
				{ "Quadratic", BufferDataType::Float },
			}, 50);

	SpotlightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Ambient",   BufferDataType::Vec3 },
				{ "Diffuse",   BufferDataType::Vec3 },
				{ "Specular",  BufferDataType::Vec3 },
				{ "Position",  BufferDataType::Vec3 },
				{ "Direction", BufferDataType::Vec3 },
				{ "CutoffAngle",	  BufferDataType::Float },
				{ "OuterCutoffAngle", BufferDataType::Float },
			}, 50);
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
	}

	BillboardBuffer->Clear(DrawBufferIndex::Instances);

	{
		DirectionalLightBillboardCommand =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		DirectionalLightBillboardCommand->UniformData
		.SetInput("u_View", camera->GetView());
		DirectionalLightBillboardCommand->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		DirectionalLightBillboardCommand->UniformData
		.SetInput("u_BillboardWidth", 10.0f);
		DirectionalLightBillboardCommand->UniformData
		.SetInput("u_BillboardHeight", 10.0f);
		DirectionalLightBillboardCommand->UniformData
		.SetInput("u_Texture", TextureSlot{ DirectionalLightIcon, 0 });

		auto& call = DirectionalLightBillboardCommand->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 0;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.DepthTest = DepthTestingMode::Off;
		call.Culling = CullingMode::Off;
		call.Blending = BlendingMode::Greatest;
	}

	{
		PointLightBillboardCommand =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		PointLightBillboardCommand->UniformData
		.SetInput("u_View", camera->GetView());
		PointLightBillboardCommand->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		PointLightBillboardCommand->UniformData
		.SetInput("u_BillboardWidth", 10.0f);
		PointLightBillboardCommand->UniformData
		.SetInput("u_BillboardHeight", 10.0f);
		PointLightBillboardCommand->UniformData
		.SetInput("u_Texture", TextureSlot{ PointLightIcon, 0 });

		auto& call = PointLightBillboardCommand->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 1;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.DepthTest = DepthTestingMode::Off;
		call.Culling = CullingMode::Off;
		call.Blending = BlendingMode::Greatest;
	}

	{
		SpotlightBillboardCommand =
			RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
		SpotlightBillboardCommand->UniformData
		.SetInput("u_View", camera->GetView());
		SpotlightBillboardCommand->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		SpotlightBillboardCommand->UniformData
		.SetInput("u_BillboardWidth", 10.0f);
		SpotlightBillboardCommand->UniformData
		.SetInput("u_BillboardHeight", 10.0f);
		SpotlightBillboardCommand->UniformData
		.SetInput("u_Texture", TextureSlot{ SpotlightIcon, 0 });

		auto& call = SpotlightBillboardCommand->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceStart = 51;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.DepthTest = DepthTestingMode::Off;
		call.Culling = CullingMode::Off;
		call.Blending = BlendingMode::Greatest;
	}

	LightingCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
	LightingCommand->Clear = false;
	LightingCommand->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	LightingCommand->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void EditorSceneRenderer::SubmitCamera(const Entity& entity) {

}

void EditorSceneRenderer::SubmitSkybox(const Entity& entity) {
	auto& sc = entity.Get<SkyboxComponent>();
	auto* assetManager = App::Get()->GetAssetManager();
	assetManager->Load(sc.CubemapAsset);
	auto cubemap = assetManager->Get<Cubemap>(sc.CubemapAsset);

	LightingCommand->UniformData
	.SetInput("u_Skybox", CubemapSlot{ cubemap });
}

void EditorSceneRenderer::SubmitLight(const Entity& entity) {
	glm::vec3 position;
	uint32_t type = 0;
	DrawCommand* command;

	if(entity.Has<DirectionalLightComponent>()) {
		auto& dc = entity.Get<DirectionalLightComponent>();
		// DirectionalLightBuffer->SetData(&dc);
		if(HasDirectionalLight)
			return;

		HasDirectionalLight = true;
		position = dc.Position;
		type = 1;
		command = DirectionalLightBillboardCommand;
	}
	else if(entity.Has<PointLightComponent>()) {
		auto& pc = entity.Get<PointLightComponent>();
		// PointLightBuffer->SetData(&pc, 1, PointLightCount++);

		position = pc.Position;
		type = 2;
		command = PointLightBillboardCommand;
	}
	else if(entity.Has<SpotlightComponent>()) {
		auto& sc = entity.Get<SpotlightComponent>();
		// SpotlightBuffer->SetData(&sc, 1, SpotlightCount++);

		position = sc.Position;
		type = 3;
		command = SpotlightBillboardCommand;
	}

	auto* buffer = BillboardBuffer;
	auto& call = command->Calls[-1];
	RendererAPI::Get()
	->SetBufferData(buffer, DrawBufferIndex::Instances, glm::value_ptr(position),
					1, call.InstanceStart + call.InstanceCount++);
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

	if(entity == Selected)
		return;

	Renderer::StartPass(LightingPass, false);
	{
		Renderer3D::DrawMesh(mesh, tc);
	}
	Renderer::EndPass();
}

void EditorSceneRenderer::Render() {
	// LightingCommand->UniformData
	// .SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	// LightingCommand->UniformData
	// .SetInput("u_PointLightCount", (int32_t)PointLightCount);
	// LightingCommand->UniformData
	// .SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

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

		auto width = (float)Application::GetWindow()->GetWidth();
		auto height = (float)Application::GetWindow()->GetHeight();

		Renderer::StartPass(OutlinePass);
		{
			auto* command = Renderer::GetCommand();
			command->UniformData
			.SetInput("u_PixelSize", 1.0f / glm::vec2(width, height));
			command->UniformData
			.SetInput("u_Color", glm::vec3(0.0f, 0.0f, 1.0f));

			auto mask = MaskPass->GetOutput();
			Renderer2D::DrawFullscreenQuad(mask, AttachmentTarget::Color);
		}
		Renderer::EndPass();

		Renderer::StartPass(LightingPass, false);
		{
			Renderer3D::DrawMesh(mesh, tc);
		}
		Renderer::EndPass();
	}

	Renderer3D::End();

	Renderer::Flush();

	HasDirectionalLight = false;
	PointLightCount = 0;
	SpotlightCount = 0;
}

}