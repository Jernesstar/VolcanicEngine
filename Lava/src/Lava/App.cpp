#include "App.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/ShaderLibrary.h>

#include <Magma/Scene/Component.h>

#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Script/ScriptObject.h>

#include <Magma/UI/UIRenderer.h>

#include <Magma/Physics/Physics.h>

#include "ScriptGlue.h"

using namespace Magma::UI;
using namespace Magma::ECS;
using namespace Magma::Script;
using namespace Magma::Physics;

namespace fs = std::filesystem;

namespace Lava {

static Ref<ScriptModule> s_AppModule;
static Ref<ScriptObject> s_AppObject;

static Scene* m_ScenePtr;
static UIPage* m_UIPtr;

struct RuntimeScreen {
	Scene World;
	UIPage UI;
	Ref<ScriptModule> Script;
	Ref<ScriptObject> ScriptObj;

	RuntimeScreen(const Screen& screen)
		: World(screen.Scene), UI(screen.UI)
	{
		Script = CreateRef<ScriptModule>(screen.Name);
	}

	~RuntimeScreen() {
		ScriptObj->Call("OnClose");

		ScriptObj.reset();
		Script.reset();
	}
};

static RuntimeScreen* s_Screen = nullptr;

static Scene* GetScene() {
	return &s_Screen->World;
}

static UIPage* GetUI() {
	return &s_Screen->UI;
}

App::App(const Project& project)
	: m_Project(project)
{
	s_Instance = this;

	ScriptEngine::RegisterSingleton("AppClass", "App", this);

	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void SetScreen(const string &in)", &App::SetScreen);
	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void PushScreen(const string &in)", &App::PushScreen);
	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void PopScreen()", &App::PopScreen);

	ScriptEngine::Get()->RegisterGlobalFunction(
		"SceneClass get_Scene() property", asFUNCTION(GetScene));
	ScriptEngine::Get()->RegisterGlobalFunction(
		"UIPageClass get_UI() property", asFUNCTION(GetUI));
}

App::~App() {

}

void App::OnLoad() {
	ScriptEngine::Get()->RegisterGlobalProperty(
		"AssetManagerClass AssetManager", m_AssetManager);

	Application::GetWindow()->SetTitle(m_Project.Name);

	s_AppModule = CreateRef<ScriptModule>(m_Project.App);
	s_AppModule->Load("./.volc.class");

	s_AppObject = s_AppModule->GetClass(m_Project.App)->Instantiate();
	s_AppObject->Call("OnLoad");

	SetScreen(m_Project.StartScreen);
}

void App::OnClose() {
	delete s_Screen;

	s_AppObject->Call("OnClose");

	s_AppObject.reset();
	s_AppModule.reset();
}

void App::OnUpdate(TimeStep ts) {
	s_AppObject->Call("OnUpdate", (float)ts);

	if(!s_Screen)
		return;

	s_Screen->ScriptObj->Call("OnUpdate", (float)ts);

	s_Screen->World.OnUpdate(ts);
	s_Screen->World.OnRender(m_SceneRenderer);

	UIRenderer::BeginFrame();

	s_Screen->UI.Traverse(
		[&](UIElement* element, TraversalStage state)
		{
			if(state == TraversalStage::Begin) {
				element->Draw();

				auto object = element->ScriptInstance;
				if(!object)
					return;

				object->Call("OnUpdate", (float)ts);

				UIState state = element->GetState();
				if(state.Clicked)
					object->Call("OnClick");
				if(state.Hovered)
					object->Call("OnHover");
				if(state.MouseUp)
					object->Call("OnMouseUp");
				if(state.MouseDown)
					object->Call("OnMouseDown");
			}
			else {
				if(element->GetType() == UIElementType::Window)
					UIRenderer::Pop(1);
			}
		});

	UIRenderer::EndFrame();
}

void App::SetScreen(const std::string& name) {
	if(!ChangeScreen) {
		Running = false;
		return;
	}
	if(name == "")
		return;

	auto [found, idx] =
		m_Project.Screens.Find(
			[name](const Screen& screen) -> bool
			{
				return screen.Name == name;
			});
	if(!found) {
		VOLCANICORE_LOG_INFO("Screen '%s' was not found", name.c_str());
		return;
	}
	else
		VOLCANICORE_LOG_INFO("Found screen '%s", name.c_str());

	auto& screen = m_Project.Screens[idx];
	delete s_Screen;
	s_Screen = new RuntimeScreen(screen);

	ScreenLoad(s_Screen->Script);
	if(screen.Scene != "")
		SceneLoad(s_Screen->World);
	if(screen.UI != "")
		UILoad(s_Screen->UI);

	auto scriptClass = s_Screen->Script->GetClass(name);
	s_Screen->ScriptObj = scriptClass->Instantiate();
	s_Screen->ScriptObj->Call("OnLoad");

	s_Screen->UI.Traverse(
		[&](UIElement* element)
		{
			if(!element->ModuleID || element->Class == "")
				return;

			Asset asset = { element->ModuleID, AssetType::Script };
			m_AssetManager->Load(asset);
			auto mod = m_AssetManager->Get<ScriptModule>(asset);
			Ref<ScriptClass> _class = mod->GetClass(element->Class);

			if(!_class)
				return;

			element->ScriptInstance = _class->Instantiate(element->GetID());
		});
}

void App::PushScreen(const std::string& name) {
	if(!ChangeScreen) {
		Running = false;
		return;
	}


}

void App::PopScreen() {
	if(!ChangeScreen) {
		Running = false;
		return;
	}


}

RuntimeSceneRenderer::RuntimeSceneRenderer() {
	auto camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	Ref<ShaderPipeline> shader;
	Ref<Framebuffer> buffer;

	shader = ShaderLibrary::Get("Lighting");
	buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass = RenderPass::Create("Lighting", shader, buffer);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderLibrary::Get("Bloom");
	buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass = RenderPass::Create("Bloom", shader, buffer);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	DirectionalLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Ambient",   BufferDataType::Vec3 },
				{ "Diffuse",   BufferDataType::Vec3 },
				{ "Specular",  BufferDataType::Vec3 },
				{ "Direction", BufferDataType::Vec3 },
			});

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
			}, 100);

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
			}, 100);
}

void RuntimeSceneRenderer::Update(TimeStep ts) {

}

void RuntimeSceneRenderer::Begin() {
	FirstCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());

}

void RuntimeSceneRenderer::SubmitCamera(Entity entity) {
	auto camera = entity.Get<CameraComponent>().Cam;

	FirstCommand->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	FirstCommand->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void RuntimeSceneRenderer::SubmitSkybox(Entity entity) {
	// auto& sc = entity.Get<SkyboxComponent>();

	// FirstCommand->UniformData
	// .SetInput("u_Skybox", CubemapSlot{ sc.Asset.Get<Cubemap>() });
}

void RuntimeSceneRenderer::SubmitLight(Entity entity) {
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

void RuntimeSceneRenderer::SubmitParticles(Entity entity) {

}

void RuntimeSceneRenderer::SubmitMesh(Entity entity) {
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();
	auto mesh = App::Get()->GetAssetManager().Get<Mesh>(mc.MeshAsset);

	if(!Objects.count(mesh))
		Objects[mesh] =
			RendererAPI::Get()->NewDrawCommand(LightingPass->Get());

	auto* command = Objects[mesh];

	Renderer3D::DrawMesh(mesh, tc, command);
}

void RuntimeSceneRenderer::Render() {
	FirstCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	FirstCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	FirstCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	Renderer3D::End();
	Objects.clear();

	Renderer::Flush();

	PointLightCount = 0;
	SpotlightCount = 0;
	HasDirectionalLight = false;
}

}