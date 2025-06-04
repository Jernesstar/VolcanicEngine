#pragma once

#include <Magma/Scene/Scene.h>

#include "Editor/Tab.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

enum class SceneState { Edit, Play, Pause };

class SceneTab : public Tab {
public:
	SceneTab();
	SceneTab(const std::string& path);
	~SceneTab();

	void Update(TimeStep ts) override;
	void Render() override;

	void SetScene(const std::string& path);
	Scene* GetScene() { return &m_Scene; }

	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveAs();

	void AddEntity();

private:
	Scene m_Scene;
	std::string m_ScenePath;
	uint32_t m_CallbackID;

private:
	void Setup();
};

}