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
	void SubmitMesh(Entity entity) override;
	void SubmitLight(Entity entity) override;
	void SubmitCamera(Entity entity) override;
	void SubmitSkybox(Entity entity) override;
	void Render() override;
};

static Ref<Renderer> s_SceneRenderer;

static Ref<ScriptModule> m_AppModule;
static Ref<ScriptClass> m_AppClass;
static Ref<ScriptObject> m_AppObject;
static Map<std::string, Ref<ScriptModule>> m_Modules;

static std::string m_CurrentScreen;
static Ref<Scene> m_CurrentScene = nullptr;

static Map<std::string, Ref<ScriptObject>> m_Objects;
static Ref<UIPage> m_CurrentPage = nullptr;

void App::OnLoad() {
	ScriptEngine::Init();

	ScriptEngine::RegisterSingleton("App", "s_App", this);
	ScriptEngine::RegisterMethod<App>("App", "void SetScreen(const string &in)", SetScreen);

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
	m_AppModule = CreateRef<ScriptModule>(m_Project.App);
	m_AppModule->Reload(appPath);

	Application::GetWindow()->SetTitle(m_Project.Name);
	// Application::PushDir(project.Path);

	m_AppClass = m_AppModule->GetScriptClass(m_Project.App);

	m_AppObject = m_AppClass->Instantiate();
	m_AppObject->Call("OnLoad");

	// SetScreen(m_Project.StartScreen);

	// s_SceneRenderer = CreateRef<Lava::Renderer>();

	UIRenderer::Init();
}

void App::OnClose() {
	m_AppObject->Call("OnClose");

	UIRenderer::Close();

	m_AppObject.reset();
	m_AppModule.reset();
	ScriptEngine::Shutdown();
}

void App::OnUpdate(TimeStep ts) {
	m_AppObject->Call("OnUpdate", (float)ts);

	if(!m_CurrentScene || !m_CurrentPage)
		return;

	if(m_CurrentScene)
		m_CurrentScene->OnRender(*s_SceneRenderer);

	UIRenderer::BeginFrame();

	if(m_CurrentPage) {
		m_CurrentPage->Render();

		m_CurrentPage->Traverse(
			[&](UIElement* element)
			{
				if(!m_Objects.count(element->GetID()))
					return;
				auto object = m_Objects[element->GetID()];

				object->Call("OnUpdate", ts);

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
	}

	UIRenderer::EndFrame();
}

void App::SetScreen(const std::string& name) {
	auto [found, idx] = m_Project.Screens.Find(
		[name](const Screen& screen) -> bool
		{
			return screen.Name == name;
		});
	if(!found) {
		VOLCANICORE_LOG_INFO("Screen '%s' was not found", name.c_str());
		return;
	}

	auto& screen = m_Project.Screens[idx];
	m_CurrentScene = CreateRef<Scene>();
	m_CurrentPage = CreateRef<UIPage>();
	SceneLoader::Load(*m_CurrentScene, screen.Scene);
	UILoader::Load(*m_CurrentPage, screen.Page);
}

Renderer::Renderer() {

}

void Renderer::Update(TimeStep ts) {

}

void Renderer::SubmitMesh(Entity entity) {

}

void Renderer::SubmitLight(Entity entity) {

}

void Renderer::SubmitCamera(Entity entity) {

}

void Renderer::SubmitSkybox(Entity entity) {

}

void Renderer::Render() {

}

}