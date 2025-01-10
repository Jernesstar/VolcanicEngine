#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include "EditorLayer.h"

using namespace VolcaniCore;

namespace Magma {

class Editor : public Application {
public:
	Editor(const CommandLineArgs& args);
	~Editor();

	void OnUpdate(TimeStep ts);

private:
	EditorLayer m_EditorLayer;
};

}