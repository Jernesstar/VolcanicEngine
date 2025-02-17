#pragma once

#include <Magma/Scene/Scene.h>

using namespace Magma;

namespace Lava {

class SceneLoader {
public:
	static void EditorLoad(Scene& scene, const std::string& path);
	static void EditorSave(const Scene& scene, const std::string& path);
	static void RuntimeSave(const Scene& scene,
							const std::string& projectPath,
							const std::string& exportPath);
	static void RuntimeLoad(Scene& scene, const std::string& projectPath);
};

}