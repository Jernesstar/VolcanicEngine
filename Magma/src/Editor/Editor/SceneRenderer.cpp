#include "SceneRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

#include "Editor/AssetManager.h"
#include "Editor/AssetImporter.h"
#include "Editor/EditorApp.h"
#include "Editor/Tab.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>

#include "Scene/SceneVisualizerPanel.h"

namespace Magma {

EditorSceneRenderer::EditorSceneRenderer() {
	Application::PushDir();

	auto camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 1.0f, 15.0f });
	camera->Resize(1920, 1080);
	camera->SetProjection(0.001f, 10'000.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 25.0f;

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	GridPass =
		RenderPass::Create("Grid",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Grid.glsl.vert",
				"Magma/assets/shaders/Grid.glsl.frag"
			}), m_Output);
	GridPass->SetData(Renderer2D::GetScreenBuffer());

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
		.MaxInstanceCount = 152
	};
	BillboardBuffer = RendererAPI::Get()->NewDrawBuffer(specs);

	BillboardPass =
		RenderPass::Create("Billboard",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Billboard.glsl.vert",
				"Magma/assets/shaders/Billboard.glsl.frag"
			}), m_Output);
	BillboardPass->SetData(BillboardBuffer);

	CameraIcon =
		AssetImporter::GetTexture("Magma/assets/icons/CameraIcon.png");
	DirectionalLightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/DirectionalLightIcon.png");
	PointLightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/PointLightIcon.png");
	SpotlightIcon =
		AssetImporter::GetTexture("Magma/assets/icons/SpotlightIcon.png");
	ParticlesIcon =
		AssetImporter::GetTexture("Magma/assets/icons/ParticlesIcon.png");

	MeshPass =
		RenderPass::Create("Mesh",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Mesh.glsl.vert",
				"Magma/assets/shaders/Mesh.glsl.frag"
			}), m_Output);
	MeshPass->SetData(Renderer3D::GetMeshBuffer());

	MaskPass =
		RenderPass::Create("Mask",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Mask.glsl.vert",
				"Magma/assets/shaders/Mask.glsl.frag"
			}), Framebuffer::Create(window->GetWidth(), window->GetHeight()));
	MaskPass->SetData(Renderer3D::GetMeshBuffer());

	OutlinePass =
		RenderPass::Create("Outline",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Outline.glsl.vert",
				"Magma/assets/shaders/Outline.glsl.frag"
			}), m_Output);
	OutlinePass->SetData(Renderer2D::GetScreenBuffer());

	LinePass =
		RenderPass::Create("Line",
			AssetImporter::GetShader({
				"Magma/assets/shaders/Line.glsl.vert",
				"Magma/assets/shaders/Line.glsl.frag"
			}), m_Output);
	LinePass->SetData(Renderer3D::GetLineBuffer());

	Application::PopDir();
}

EditorSceneRenderer::~EditorSceneRenderer() {
	RendererAPI::Get()->ReleaseBuffer(BillboardBuffer);
}

void EditorSceneRenderer::Update(TimeStep ts) {
	if(Hovered)
		m_Controller.OnUpdate(ts);
}

void EditorSceneRenderer::SetContext(SceneVisualizerPanel* panel) {
	RootPanel = panel;
	Renderer3D::End();
	Renderer2D::End();
	Renderer3D::GetMeshBuffer()->Clear();
	Renderer3D::GetCubemapBuffer()->Clear();
	Renderer3D::GetLineBuffer()->Clear();
}

void EditorSceneRenderer::AddBillboard(const glm::vec3& pos, uint32_t type) {
	glm::vec3 cameraPos = m_Controller.GetCamera()->GetPosition();
	Billboards.Add({ pos, type });

	// float distance = glm::distance(cameraPos, pos);

	// // Put them in the list farthest to closest
	// auto [found, i] =
	// 	Billboards.FindLast( // The minimal distance pair with greater distance
	// 		[=](const std::pair<glm::vec3, uint32_t>& pair) -> bool
	// 		{
	// 			return distance < glm::distance(pair.first, cameraPos);
	// 		});

	// if(!found) // Farthest thing
	// 	Billboards.Insert(0, { pos, type });
	// else
	// 	Billboards.Insert(i + 1, { pos, type });
}

void EditorSceneRenderer::Begin() {
	auto camera = m_Controller.GetCamera();

	{
		MeshCommand =
			RendererAPI::Get()->NewDrawCommand(MeshPass->Get());
		MeshCommand->Clear = true;
		MeshCommand->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		MeshCommand->UniformData
		.SetInput("u_CameraPosition", camera->GetPosition());
	}

	BillboardBuffer->Clear(DrawBufferIndex::Instances);
	Billboards.Clear();

	glm::vec3 pos = camera->GetPosition();
	glm::vec3 dir = camera->GetDirection();
	glm::vec3 planePos = glm::vec3(0.0f);
	glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	// float t;
	// if(glm::intersectRayPlane(pos, dir, planePos, planeNormal, t))
	// 	AddBillboard(pos + t * dir, 0);
		AddBillboard(glm::vec3(0.0f), 0);

	LineCommand = RendererAPI::Get()->NewDrawCommand(LinePass->Get());
	LineCommand->DepthTest = DepthTestingMode::On;
	LineCommand->Blending = BlendingMode::Greatest;
	LineCommand->Culling = CullingMode::Off;
	LineCommand->UniformData
	.SetInput("u_ViewProj", m_Controller.GetCamera()->GetViewProjection());
}

void EditorSceneRenderer::SubmitCamera(const Entity& entity) {
	auto camera = entity.Get<CameraComponent>().Cam;
	if(!camera)
		return;

	HasCamera = true;
	AddBillboard(camera->GetPosition(), 1);

	if(Selected != entity)
		return;

	glm::mat4 inverse = glm::inverse(camera->GetViewProjection());

	glm::vec4 p0 = inverse * glm::vec4(-1, -1, -1, 1); // Front bottom left
	glm::vec4 p1 = inverse * glm::vec4( 1, -1, -1, 1); // Front bottom right
	glm::vec4 p2 = inverse * glm::vec4( 1,  1, -1, 1); // Front top right
	glm::vec4 p3 = inverse * glm::vec4(-1,  1, -1, 1); // Front top left
	glm::vec4 p4 = inverse * glm::vec4(-1, -1,  1, 1); // Back bottom left
	glm::vec4 p5 = inverse * glm::vec4( 1, -1,  1, 1); // Back bottom right
	glm::vec4 p6 = inverse * glm::vec4( 1,  1,  1, 1); // Back top right
	glm::vec4 p7 = inverse * glm::vec4(-1,  1,  1, 1); // Back top left

	Point points[] =
	{
		{ p0 / p0.w, glm::vec3(1.0f) },
		{ p1 / p1.w, glm::vec3(1.0f) },
		{ p2 / p2.w, glm::vec3(1.0f) },
		{ p3 / p3.w, glm::vec3(1.0f) },
		{ p4 / p4.w, glm::vec3(1.0f) },
		{ p5 / p5.w, glm::vec3(1.0f) },
		{ p6 / p6.w, glm::vec3(1.0f) },
		{ p7 / p7.w, glm::vec3(1.0f) },
		{ camera->GetPosition(), glm::vec3(1.0f) }
	};

	constexpr uint32_t indexCount = 24;
	uint32_t indices[indexCount] =
	{
		8, 7, // Left Top
		8, 6, // Right Top
		8, 4, // Left Bottom
		8, 5, // Right Bottom

		3, 2, // Front Top
		0, 1, // Front Bottom
		3, 0, // Front Left
		2, 1, // Front Right

		7, 6, // Back Top
		4, 5, // Back Bottom
		7, 4, // Back Left
		6, 5, // Back Right
	};

	auto* buffer = LinePass->Get()->BufferData;
	RendererAPI::Get()
	->SetBufferData(buffer, DrawBufferIndex::Vertices, points, 9, 0);
	RendererAPI::Get()
	->SetBufferData(buffer, DrawBufferIndex::Indices, indices, indexCount);

	auto& call = LineCommand->NewDrawCall();
	call.VertexCount = 9;
	call.IndexCount = indexCount;
	call.Partition = PartitionType::Single;
	call.Primitive = PrimitiveType::Line;
}

void EditorSceneRenderer::SubmitSkybox(const Entity& entity) {
	auto& sc = entity.Get<SkyboxComponent>();
	auto* assetManager = AssetManager::Get();
	assetManager->Load(sc.CubemapAsset);
	auto cubemap = assetManager->Get<Cubemap>(sc.CubemapAsset);

	MeshCommand->UniformData
	.SetInput("u_Skybox", CubemapSlot{ cubemap });
}

void EditorSceneRenderer::SubmitLight(const Entity& entity) {
	glm::vec3 position;

	if(entity.Has<DirectionalLightComponent>()) {
		position = entity.Get<DirectionalLightComponent>().Position;
		HasDirectionalLight = true;
		AddBillboard(position, 2);
	}
	else if(entity.Has<PointLightComponent>()) {
		position = entity.Get<PointLightComponent>().Position;
		PointLightCount++;
		AddBillboard(position, 3);
	}
	else if(entity.Has<SpotlightComponent>()) {
		position = entity.Get<SpotlightComponent>().Position;
		SpotlightCount++;
		AddBillboard(position, 4);
	}
}

void EditorSceneRenderer::SubmitParticles(const Entity& entity) {
	glm::vec3 position = entity.Get<ParticleEmitterComponent>().Position;
	AddBillboard(position, 5);
}

void EditorSceneRenderer::SubmitMesh(const Entity& entity) {
	auto* assetManager = AssetManager::Get();
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();

	if(!assetManager->IsValid(mc.MeshSourceAsset))
		return;

	assetManager->Load(mc.MeshSourceAsset);
	auto mesh = assetManager->Get<Mesh>(mc.MeshSourceAsset);

	if(!mc.MaterialAsset.ID) {
		Renderer::StartPass(MeshPass);
		{
			Renderer3D::DrawMesh(mesh, tc);
		}
		Renderer::EndPass();
		return;
	}

	if(!assetManager->IsValid(mc.MaterialAsset))
		return;

	VolcaniCore::Material mat;
	assetManager->Load(mc.MaterialAsset);
	auto material = assetManager->Get<Magma::Material>(mc.MaterialAsset);

	if(material->TextureUniforms.count("u_Diffuse")) {
		UUID id = material->TextureUniforms["u_Diffuse"];
		Asset textureAsset = { id, AssetType::Texture };
		assetManager->Load(textureAsset);
		mat.Diffuse = assetManager->Get<Texture>(textureAsset);
	}
	if(material->Vec4Uniforms.count("u_DiffuseColor"))
		mat.DiffuseColor = material->Vec4Uniforms["u_DiffuseColor"];

	DrawCommand* command =
		RendererAPI::Get()->NewDrawCommand(MeshPass->Get());
	command->UniformData
	.SetInput("u_Material.IsTextured", (bool)mat.Diffuse);
	command->UniformData
	.SetInput("u_Material.Diffuse", TextureSlot{ mat.Diffuse, 0 });
	command->UniformData
	.SetInput("u_Material.DiffuseColor", mat.DiffuseColor);

	Renderer3D::DrawMesh(mesh, tc, command);
}

void EditorSceneRenderer::Render() {
	Renderer3D::End();

	auto* assetManager = AssetManager::Get();
	if(Selected && Selected.Has<TransformComponent>()
	&& Selected.Has<MeshComponent>()
	&& assetManager->IsValid(Selected.Get<MeshComponent>().MeshSourceAsset))
	{
		auto& tc = Selected.Get<TransformComponent>();
		auto& mc = Selected.Get<MeshComponent>();

		assetManager->Load(mc.MeshSourceAsset);
		auto mesh = assetManager->Get<Mesh>(mc.MeshSourceAsset);

		{
			auto* command =
				RendererAPI::Get()->NewDrawCommand(MaskPass->Get());
			command->Clear = true;
			command->UniformData
			.SetInput("u_ViewProj",
				MeshCommand->UniformData.Mat4Uniforms["u_ViewProj"]);
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
	for(auto [pos, type] : Billboards) {
		DrawCommand* command;
		if(type == 0) {
			command = RendererAPI::Get()->NewDrawCommand(GridPass->Get());
			command->UniformData
			.SetInput("u_CameraPosition", camera->GetPosition());
		}
		else {
			command = RendererAPI::Get()->NewDrawCommand(BillboardPass->Get());
			command->UniformData
			.SetInput("u_View", camera->GetView());
			command->UniformData
			.SetInput("u_BillboardWidth", 1.0f);
			command->UniformData
			.SetInput("u_BillboardHeight", 1.0f);
			Ref<Texture> icon;
			if(type == 1)
				icon = CameraIcon;
			else if(type == 2)
				icon = DirectionalLightIcon;
			else if(type == 3)
				icon = PointLightIcon;
			else if(type == 4)
				icon = SpotlightIcon;
			else if(type == 5)
				icon = ParticlesIcon;

			command->UniformData
			.SetInput("u_Texture", TextureSlot{ icon, 0 });
		}

		command->DepthTest = DepthTestingMode::On;
		command->Blending = BlendingMode::Greatest;
		command->Culling = CullingMode::Off;
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Single;

		if(type != 0) {
			call.Partition = PartitionType::Instanced;
			call.InstanceStart = BillboardBuffer->InstancesCount;
			call.InstanceCount = 1;
			BillboardBuffer->AddInstance(glm::value_ptr(pos));
		}
	}

#ifdef MAGMA_PHYSICS
	auto* scene = RootPanel->GetPhysicsWorld().Get();
	const PxRenderBuffer& rb = scene->getRenderBuffer();
	if(rb.getNbLines()) {
		List<Point> points(rb.getNbLines() * 2);
		List<uint32_t> indices(rb.getNbLines() * 2);
	
		for(uint32_t i = 0; i < rb.getNbLines(); i++) {
			const PxDebugLine& l = rb.getLines()[i];
			Point p0 = { { l.pos0.x, l.pos0.y, l.pos0.z }, glm::vec3(1.0f) };
			Point p1 = { { l.pos1.x, l.pos1.y, l.pos1.z }, glm::vec3(1.0f) };
			points.Add(p0);
			points.Add(p1);
			indices.Add(2*i);
			indices.Add(2*i + 1);
		}

		auto* buffer = LinePass->Get()->BufferData;
		RendererAPI::Get()
		->SetBufferData(buffer, DrawBufferIndex::Indices,
			indices.GetBuffer().Get(), indices.Count(), 24);
		RendererAPI::Get()
		->SetBufferData(buffer, DrawBufferIndex::Vertices,
			points.GetBuffer().Get(), points.Count(), 9);

		auto& call = LineCommand->NewDrawCall();
		call.IndexStart = 24;
		call.IndexCount = indices.Count();
		call.VertexStart = 9;
		call.VertexCount = points.Count();
		call.Partition = PartitionType::Single;
		call.Primitive = PrimitiveType::Line;
	}

	// auto* editor = RootPanel->GetTab()
	// 	->GetPanel("ComponentEditor")->As<ComponentEditorPanel>();
	// if(Selected && Selected.Has<RigidBodyComponent>()
	// && editor->IsFocused<RigidBodyComponent>(Selected))
	// {
		
	// }

#endif

	Renderer::Flush();

	HasCamera = false;
	HasDirectionalLight = false;
	PointLightCount = 0;
	SpotlightCount = 0;
}

}