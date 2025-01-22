#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include "Editor.h"

using namespace VolcaniCore;

namespace Magma {

class EditorApp : public Application {
public:
	EditorApp(const CommandLineArgs& args);
	~EditorApp();

	void OnUpdate(TimeStep ts);

	Editor& GetEditor() { return m_Editor; }

private:
	Editor m_Editor;
};

}