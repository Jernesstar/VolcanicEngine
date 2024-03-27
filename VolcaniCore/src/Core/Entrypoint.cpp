#include "Application.h"

using namespace VolcaniCore;

extern Application* CreateApplication(const std::string& project);

int main(int argc, char** argv)
{
	Application::Init();

	Application* app = CreateApplication(argv[0]);
	Application::Run();

	Application::Close();
}