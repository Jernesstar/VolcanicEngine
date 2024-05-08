#include <iostream>

#include <Core/Application.h>

#include "Demos/Model.h"
#include "Demos/Lighting.h"
#include "Demos/Empty.h"

using namespace VolcaniCore;

Application* CreateApplication() {
	return new LightingDemo();
}
