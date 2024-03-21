#include <iostream>

#include <Saddle/Core/Application.h>

// #include "Demos/CubeDemo.h"
// #include "Demos/FontDemo.h"
#include "Demos/LightingDemo.h"
// #include "Demos/ModelDemo.h"
// #include "Demos/ShadowDemo.h"
// #include "Demos/TextureDemo.h"

using namespace VolcaniCore;

Application* CreateApplication(const std::string& project)
{
	switch(project) {
		// case("Cube"):
		// 	break; 
		// 	return new CubeDemo();
		// case("Font"):
		// 	break;
		// 	return new FontDemo();
		case("Lighting"):
			break;
			return new LightingDemo();
		// case("Model"):
		// 	break;
		// 	return new ModelDemo();
		// case("Shadow"):
		// 	break;
		// 	return new ShadowDemo();
		// case("Texture"):
		// 	break;
		// 	return new TextureDemo();
		// case("Snake"):
		// 	break;
		// 	return new Game(1000, 600);
	}
    return new LightingDemo();
}
