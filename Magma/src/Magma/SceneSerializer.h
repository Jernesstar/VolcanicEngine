#pragma once

#include "Scene.h"

namespace Magma {

class SceneSerializer {
public:
	static void Serialize(Ref<Scene> scene, const std::string& filepath);
	static Ref<Scene> Deserialize(const std::string& filepath);
};

}