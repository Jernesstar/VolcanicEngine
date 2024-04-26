#include <iostream>

#include <Core/Application.h>

#include "Demos/ModelDemo.h"
#include "Demos/LightingDemo.h"

using namespace VolcaniCore;

Application* CreateApplication()
{
	return new LightingDemo();
}
