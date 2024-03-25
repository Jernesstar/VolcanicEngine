#include <iostream>

#include <Core/Application.h>

// #include "Demos/CubeDemo.h"
// #include "Demos/FontDemo.h"
#include "Demos/LightingDemo.h"
// #include "Demos/ModelDemo.h"
// #include "Demos/ShadowDemo.h"
// #include "Demos/TextureDemo.h"

using namespace VolcaniCore;

Application* CreateApplication(const std::string& project)
{
	return new LightingDemo();
}
