#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Editor/Editor.h>

Application* CreateApplication(const CommandLineArgs& args) {
	return new Magma::Editor();
}