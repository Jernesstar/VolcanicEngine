#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <Magma/Core/Project.h>
#include <Magma/Core/AssetManager.h>
#include <Magma/Scene/Scene.h>
#include <Magma/UI/UIPage.h>
#include <Magma/Script/ScriptModule.h>

#include "SceneRenderer.h"

using namespace VolcaniCore;
using namespace Magma;
using namespace Magma::UI;
using namespace Magma::Script;

namespace Lava {

class App {
public:
	static App* Get() { return s_Instance; }

public:
	bool ChangeScreen;
	bool RenderScene;
	bool RenderUI;
	bool Running;

	Func<void, Ref<ScriptModule>&> AppLoad;
	Func<void, Ref<ScriptModule>&, const std::string&> ScreenLoad;
	Func<void, Scene&> SceneLoad;
	Func<void, UIPage&> UILoad;

public:
	App();
	~App() = default;

	void SetProject(const Project& project);

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void LoadScene(Scene* scene);
	void LoadUI(UIPage* ui);
	Scene* GetScene();
	UIPage* GetUI();

	void SetCurrentEntity(Entity entity);
	Entity GetCurrentEntity();

	void SwitchScreen(const std::string& name);
	void PushScreen(const std::string& name);
	void PopScreen(const std::string& name);

	void ScreenSet(const std::string& name);
	void ScreenPush(const std::string& name);
	void ScreenPop();

	auto& GetProject() { return m_Project; }
	RuntimeSceneRenderer& GetRenderer() { return m_SceneRenderer; }

private:
	Project m_Project;
	RuntimeSceneRenderer m_SceneRenderer;

private:
	inline static App* s_Instance;
};

}