#include "App.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Script/ScriptObject.h>

#include <Magma/UI/UIRenderer.h>

#include <Lava/ScriptGlue.h>

#include "SceneLoader.h"
#include "UILoader.h"

using namespace Magma::UI;
using namespace Magma::ECS;
using namespace Magma::Script;

namespace fs = std::filesystem;

namespace Lava {

static Ref<ScriptModule> s_AppModule;
static Ref<ScriptClass> s_AppClass;
static Ref<ScriptObject> s_AppObject;

struct RuntimeScreen {
	Scene CurrentScene;
	UIPage CurrentPage;

	RuntimeScreen(const Screen& screen)
		: CurrentScene(screen.Scene), CurrentPage(screen.Page) { }
};

static RuntimeScreen* s_CurrentScreen;

static Ref<ScriptModule> s_Module;
static List<Ref<ScriptClass>> s_Classes;
static Map<std::string, Ref<ScriptObject>> s_Objects;

void App::OnLoad() {
	ScriptEngine::Init();
	Lava::ScriptGlue::Init();

	ScriptEngine::RegisterSingleton("App", "s_App", this);
	ScriptEngine::RegisterMethod<App>(
		"App", "void SetScreen(const string &in)", &App::SetScreen);

	auto appPath = (fs::path(m_Project.ExportPath) / ".volc.class").string();
	s_AppModule = CreateRef<ScriptModule>(m_Project.App);
	s_AppModule->Reload(appPath);

	Application::GetWindow()->SetTitle(m_Project.Name);

	s_AppClass = s_AppModule->GetScriptClass(m_Project.App);

	s_AppObject = s_AppClass->Instantiate();
	s_AppObject->Call("OnLoad");

	SetScreen(m_Project.StartScreen);

	UIRenderer::Init();
}

void App::OnClose() {
	UIRenderer::Close();

	s_AppObject->Call("OnClose");

	s_Objects.clear();
	s_Classes.Clear();

	s_Module.reset();
	s_AppObject.reset();
	s_AppModule.reset();
	ScriptEngine::Shutdown();
}

void App::OnUpdate(TimeStep ts) {
	s_AppObject->Call("OnUpdate", (float)ts);

	if(!s_CurrentScreen)
		return;

	s_CurrentScreen->CurrentScene.OnRender(m_SceneRenderer);

	UIRenderer::BeginFrame();

	s_CurrentScreen->CurrentPage.Traverse(
		[&](UIElement* element, TraversalStage state)
		{
			if(state == TraversalStage::Begin) {
				element->Draw();

				if(!s_Objects.count(element->GetID()))
					return;
				auto object = s_Objects[element->GetID()];
				
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

	auto& screen = m_Project.Screens[idx];
	delete s_CurrentScreen;
	s_CurrentScreen = new RuntimeScreen(screen);

	SceneLoader::Load(s_CurrentScreen->CurrentScene, m_Project.ExportPath);

	s_Module =
		UILoader::Load(s_CurrentScreen->CurrentPage, m_Project.ExportPath);

	s_CurrentScreen->CurrentPage.Traverse(
		[](UIElement* element)
		{
			Ref<ScriptClass> scriptClass =
				s_Module->GetScriptClass(element->GetID());

			if(!scriptClass)
				return;

			s_Classes.Add(scriptClass);
			s_Objects[element->GetID()] = scriptClass->Instantiate();
		});
}

RuntimeSceneRenderer::RuntimeSceneRenderer() {

}

void RuntimeSceneRenderer::Begin() {

}

void RuntimeSceneRenderer::Update(TimeStep ts) {

}

void RuntimeSceneRenderer::SubmitCamera(Entity entity) {

}

void RuntimeSceneRenderer::SubmitSkybox(Entity entity) {

}

void RuntimeSceneRenderer::SubmitLight(Entity entity) {

}

void RuntimeSceneRenderer::SubmitMesh(Entity entity) {

}

void RuntimeSceneRenderer::Render() {

}

}