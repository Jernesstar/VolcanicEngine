#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Editor/EditorApp.h>

Application* CreateApplication(const CommandLineArgs& args) {
	return new Magma::EditorApp(args);
}