#pragma once

#include <vector>

#include "Core/Defines.h"
#include "Camera/Camera.h"

#include "Mesh.h"

namespace VolcaniCore {

class Light;

class Scene {
public:

private:
	Ref<Camera> m_Camera;
	std::vector<Light> m_Lights;
	std::vector<Mesh> m_Meshes;
};


}