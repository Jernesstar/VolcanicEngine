#pragma once

#include <VolcaniCore/Camera/Camera.h>

#include "Defines.h"
#include "EntitySystem.h"

namespace Magma {

class Scene {
public:
	Scene();
	~Scene();

private:
	EntitySystem m_EntitySystem;
	Ref<VolcaniCore::Camera> m_SceneCamera;
};

}