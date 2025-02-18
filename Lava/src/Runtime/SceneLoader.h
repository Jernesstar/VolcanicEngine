#pragma once

#include <Magma/Scene/Scene.h>

using namespace Magma;

namespace Lava {

class SceneLoader {
public:
	static void Save(const Scene& scene, const std::string& path);
	static void Load(Scene& scene, const std::string& projectPath);
};

}