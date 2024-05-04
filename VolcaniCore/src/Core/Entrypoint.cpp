#include "Application.h"

using namespace VolcaniCore;

extern Application* CreateApplication();

int main(int argc, char** argv) {
	Application::Init();

	Application* app = CreateApplication();
	Application::Run();

	Application::Close();
}