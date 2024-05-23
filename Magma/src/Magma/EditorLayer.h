#pragma once

#include <VolcaniCore/Core/Time.h>

#include "Scene.h"

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(VolcaniCore::TimeStep ts);
	void Render();

private:
	Ref<Scene> m_CurrentScene;

	void NewScene();
	void OpenScene();
	void SaveScene();
};

}