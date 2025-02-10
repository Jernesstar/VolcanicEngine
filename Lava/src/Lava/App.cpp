#include "App.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>

#include <Magma/Core/DLL.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>
#include <Magma/Scene/SceneRenderer.h>

#include <Magma/UI/UIRenderer.h>

#include "SceneLoader.h"
#include "UILoader.h"

using namespace Magma::UI;

namespace fs = std::filesystem;

namespace Lava {

class Renderer : public SceneRenderer {
public:
	Renderer();
	~Renderer() = default;

	void Update(TimeStep ts) override;

	void Begin() override;
	void SubmitCamera(Entity entity) override;
	void SubmitSkybox(Entity entity) override;
	void SubmitLight(Entity entity) override;
	void SubmitMesh(Entity entity) override;
	void Render() override;
};

static Ref<Renderer> s_SceneRenderer;

static Ref<ScriptModule> s_AppModule;
static Ref<ScriptClass> s_AppClass;
static Ref<ScriptObject> s_AppObject;

struct RuntimeScreen {
	Scene Scene;
	UIPage Page;
};

static RuntimeScreen* s_CurrentScreen;

static Ref<ScriptModule> s_Module;
static List<Ref<ScriptClass>> s_Classes;
static Map<std::string, Ref<ScriptObject>> s_Objects;
static Theme s_Theme;

void App::OnLoad() {
	ScriptEngine::Init();

	ScriptEngine::RegisterSingleton("App", "s_App", this);
	ScriptEngine::RegisterMethod<App>("App", "void SetScreen(const string &in)", &App::SetScreen);

	ScriptEngine::RegisterInterface("IApp")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

	auto appPath =
		(fs::path(m_Project.Path) / "Project" / "App" / m_Project.App).string() + ".as";
	s_AppModule = CreateRef<ScriptModule>(m_Project.App);
	s_AppModule->Reload(appPath);

	Application::GetWindow()->SetTitle(m_Project.Name);
	// Application::PushDir(project.Path);

	s_AppClass = s_AppModule->GetScriptClass(m_Project.App);

	s_AppObject = s_AppClass->Instantiate();
	s_AppObject->Call("OnLoad");

	auto themePath =
		(fs::path(m_Project.Path) / "Visual" / "UI" / "Page" / "theme.magma.ui.json").string();
	if(FileUtils::FileExists(themePath))
		s_Theme = UILoader::LoadTheme(themePath);

	SetScreen(m_Project.StartScreen);

	UIRenderer::Init();

	s_SceneRenderer = CreateRef<Lava::Renderer>();
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

	s_CurrentScreen->Scene.OnRender(*s_SceneRenderer);

	UIRenderer::BeginFrame();

	s_CurrentScreen->Page.Render();
	s_CurrentScreen->Page.Traverse(
		[&](UIElement* element)
		{
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
	s_CurrentScreen = new RuntimeScreen;

	auto scenePath = fs::path(m_Project.Path) / "Visual" / "Scene" / "Schema" / screen.Scene;
	SceneLoader::Load(s_CurrentScreen->Scene, scenePath.string() + ".magma.scene");

	auto pagePath = fs::path(m_Project.Path) / "Visual" / "UI" / "Page" / screen.Page;
	s_CurrentScreen->Page.SetTheme(s_Theme);
	UILoader::Load(s_CurrentScreen->Page, pagePath.string());
	UILoader::Compile(pagePath.string());

	s_Module = UILoader::GetModule(screen.Page);
	s_CurrentScreen->Page.Traverse(
		[](UIElement* element)
		{
			Ref<ScriptClass> scriptClass =
				s_Module->GetScriptClass(element->GetID());

			if(!scriptClass)
				return;

			s_Classes.Add(scriptClass);
			s_Objects[element->GetID()] =
				scriptClass->Instantiate();
		});
}

Renderer::Renderer() {

}

void Renderer::Begin() {

}

void Renderer::Update(TimeStep ts) {

}

void Renderer::SubmitCamera(Entity entity) {

}

void Renderer::SubmitSkybox(Entity entity) {

}

void Renderer::SubmitLight(Entity entity) {

}

void Renderer::SubmitMesh(Entity entity) {

}

void Renderer::Render() {

}

}