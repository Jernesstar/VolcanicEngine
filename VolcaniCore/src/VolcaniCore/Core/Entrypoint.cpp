#include "Application.h"
#include "CommandLineArgs.h"

using namespace VolcaniCore;

extern Application* CreateApplication(const CommandLineArgs& args);

int main(int argc, char** argv) {
	Application::SetCurrentDir();
	Application::Init();
	Application* app = CreateApplication(CommandLineArgs(argc, argv));
	Application::Run();
	Application::Close();
}