#pragma once

#include <VolcaniCore/Core/Application.h>

#include "EditorLayer.h"

using namespace VolcaniCore;

namespace Magma {

class Editor : public Application {
public:
	Editor();
	~Editor();

	void OnUpdate(TimeStep ts);

private:
	Ref<EditorLayer> m_EditorLayer;
	Ref<Camera> m_Camera;
};

}