#include "App.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>

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

static Ref<ScriptModule> s_AppModule = nullptr;
static Ref<ScriptObject> s_AppObject = nullptr;

static bool s_ScreenPrepared = false;
static bool s_ShouldSwitchScreen = false;
static bool s_ShouldPushScreen = false;
static bool s_ShouldPopScreen = false;
static std::string s_NewScreenName = "";

static Scene* s_ShouldLoadScene = nullptr;
static UIPage* s_ShouldLoadUI = nullptr;

struct RuntimeScreen {
	Ref<Scene> World;
	UIPage UI;
	Ref<ScriptModule> Script;
	Ref<ScriptObject> ScriptObj;

	RuntimeScreen()
		: World(CreateRef<Scene>("")), UI("")
	{
	}

	~RuntimeScreen() {
		ScriptObj->Call("OnClose");
		ScriptObj.reset();
		Script.reset();

		App::Get()->GetRenderer().OnSceneClose();
		UI.Clear();
		World->UnregisterSystems();
		World.reset();
	}
};

static RuntimeScreen* s_Screen = nullptr;

static Scene& ScriptGetScene() {
	return *s_Screen->World;
}

static UIPage& ScriptGetUI() {
	return s_Screen->UI;
}

static asIScriptObject* GetScriptApp() {
	auto* handle = s_AppObject->GetHandle();
	handle->AddRef();
	return handle;
}

static AssetManager& GetAssetManagerInstance() {
	return *AssetManager::Get();
}

static void ScriptLoadScene(const std::string& name, App* app) {
	s_Screen->World->Name = name;
	s_Screen->World->EntityWorld.Reset();
	s_Screen->World->UnregisterSystems();
	s_Screen->World->RegisterSystems();
	app->GetRenderer().OnSceneLoad();
	app->SceneLoad(*s_Screen->World);

	List<Entity> list;
	s_Screen->World->EntityWorld
	.ForEach<ScriptComponent>(
		[&](Entity entity)
		{
			auto& sc = entity.Set<ScriptComponent>();
			if(sc.Instance)
				list.Add(entity);
		});

	list.ForEach(
		[](Entity& entity)
		{
			auto& sc = entity.Set<ScriptComponent>();
			auto old = sc.Instance;
			if(!old->IsInitialized()) { // i.e Editor
				sc.Instance = old->GetClass()->Instantiate(entity);
				ScriptGlue::Copy(old, sc.Instance);
			}

			sc.Instance->Call("OnStart");
		});

}

static void ScriptLoadUI(const std::string& name, App* app) {
	s_Screen->UI.Name = name;
	app->UILoad(s_Screen->UI);
}

App::App() {
	s_Instance = this;

	ScriptEngine::RegisterSingleton("AppClass", "App", this);

	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void SwitchScreen(const string &in)", &App::SwitchScreen);
	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void PushScreen(const string &in)", &App::PushScreen);
	ScriptEngine::RegisterMethod<App>(
		"AppClass", "void PopScreen()", &App::PopScreen);
	ScriptEngine::Get()->RegisterObjectMethod(
		"AppClass", "void LoadScene(const string &in)",
		asFUNCTION(ScriptLoadScene), asCALL_CDECL_OBJLAST);
	ScriptEngine::Get()->RegisterObjectMethod(
		"AppClass", "void LoadUI(const string &in)",
		asFUNCTION(ScriptLoadUI), asCALL_CDECL_OBJLAST);

	ScriptEngine::Get()->RegisterGlobalFunction(
		"SceneClass& get_Scene() property",
		asFUNCTION(ScriptGetScene), asCALL_CDECL);
	ScriptEngine::Get()->RegisterGlobalFunction(
		"UIPageClass& get_UIPage() property",
		asFUNCTION(ScriptGetUI), asCALL_CDECL);

	ScriptEngine::Get()->RegisterGlobalFunction(
		"IApp@ get_ScriptApp() property",
		asFUNCTION(GetScriptApp), asCALL_CDECL);
	ScriptEngine::Get()->RegisterGlobalFunction(
		"AssetManagerClass& get_AssetManager() property",
		asFUNCTION(GetAssetManagerInstance), asCALL_CDECL);
}

void App::SetProject(const Project& project) {
	m_Project = project;
}

void App::PrepareScreen() {
	s_ScreenPrepared = true;
	delete s_Screen;
	s_Screen = new RuntimeScreen();
}

void App::OnLoad() {
	s_AppModule = CreateRef<ScriptModule>();
	AppLoad(s_AppModule);

	s_AppObject = s_AppModule->GetClass(m_Project.App)->Instantiate();
	s_AppObject->Call("OnLoad");
}

void App::OnClose() {
	delete s_Screen;
	s_Screen = nullptr;
	s_ScreenPrepared = false;

	if(s_AppObject)
		s_AppObject->Call("OnClose");

	s_AppObject.reset();
	s_AppModule.reset();
}

void App::OnUpdate(TimeStep ts) {
	if(!Running)
		return;

	if(s_ShouldSwitchScreen)
		ScreenSet(s_NewScreenName);
	else if(s_ShouldPushScreen)
		ScreenPush(s_NewScreenName);
	else if(s_ShouldPopScreen)
		ScreenPop();

	s_AppObject->Call("OnUpdate", (float)ts);

	if(!s_Screen)
		return;

	s_Screen->ScriptObj->Call("OnUpdate", (float)ts);

	s_Screen->World->OnUpdate(ts);
	m_SceneRenderer.Update(ts);
	s_Screen->World->OnRender(m_SceneRenderer);

	if(RenderScene) {
		auto output = m_SceneRenderer.GetOutput();
		Renderer2D::DrawFullscreenQuad(output, AttachmentTarget::Color);
		Renderer::Flush();
	}

	if(!RenderUI)
		return;

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
}

void App::LoadScene(Scene* scene) {
	s_ShouldLoadScene = scene;
}

void App::LoadUI(UIPage* ui) {
	s_ShouldLoadUI = ui;
}

Scene* App::GetScene() {
	VOLCANICORE_ASSERT(s_Screen);
	return s_Screen->World.get();
}

UIPage* App::GetUI() {
	VOLCANICORE_ASSERT(s_Screen);
	return &s_Screen->UI;
}

void App::SwitchScreen(const std::string& name) {
	if (!ChangeScreen) {
		Running = false;
		return;
	}

	s_ShouldSwitchScreen = true;
	s_NewScreenName = name;
}

void App::PushScreen(const std::string& name) {
	if (!ChangeScreen) {
		Running = false;
		return;
	}

	s_ShouldPushScreen = true;
	s_NewScreenName = name;
}

void App::PopScreen(const std::string& name) {
	if (!ChangeScreen) {
		Running = false;
		return;
	}

	s_ShouldPopScreen = true;
	s_NewScreenName = name;
}

void App::ScreenSet(const std::string& name) {
	s_ShouldSwitchScreen = false;
	if(name == "")
		return;

	auto [found, idx] =
		m_Project.Screens.Find(
			[name](const Screen& screen) { return screen.Name == name; });
	if(!found) {
		VOLCANICORE_LOG_INFO("Screen '%s' was not found", name.c_str());
		return;
	}
	else
		VOLCANICORE_LOG_INFO("Found screen '%s'", name.c_str());

	auto& screen = m_Project.Screens[idx];
	if(!s_ScreenPrepared)
		PrepareScreen();
	s_ScreenPrepared = false;

	s_Screen->World->Name = screen.Scene;
	s_Screen->UI.Name = screen.UI;

	ScreenLoad(s_Screen->Script, screen.Name);
	auto scriptClass = s_Screen->Script->GetClass(name);
	s_Screen->ScriptObj = scriptClass->Instantiate();

	if(s_ShouldLoadScene) {
		m_SceneRenderer.OnSceneLoad();
		s_Screen->World->RegisterSystems();
		*s_Screen->World = *s_ShouldLoadScene;
		s_ShouldLoadScene = nullptr;

		List<Entity> list;
		s_Screen->World->EntityWorld
		.ForEach<ScriptComponent>(
			[&](Entity entity)
			{
				const auto& sc = entity.Get<ScriptComponent>();
				if(sc.Instance)
					list.Add(entity);
			});

		list.ForEach(
			[](Entity& entity)
			{
				auto& sc = entity.Set<ScriptComponent>();
				auto old = sc.Instance;
				sc.Instance = old->GetClass()->Instantiate(entity);
				ScriptGlue::Copy(old, sc.Instance);
				sc.Instance->Call("OnStart");
			});

	}
	else if(screen.Scene != "")
		ScriptLoadScene(screen.Scene, this);

	if(s_ShouldLoadUI) {
		s_Screen->UI = *s_ShouldLoadUI;
		s_ShouldLoadUI = nullptr;
	}
	else if(screen.UI != "")
		UILoad(s_Screen->UI);

	s_Screen->ScriptObj->Call("OnLoad");
}

void App::ScreenPush(const std::string& name) {

}

void App::ScreenPop() {

}

}