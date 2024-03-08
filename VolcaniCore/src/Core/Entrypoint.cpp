#include "Application.h"

using namespace VolcaniCore;

int main(int argc, char** argv)
{
	Application::Init();

	Application* app = new Application();
	app->Run();

	Application::Close();
}