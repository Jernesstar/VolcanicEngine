#pragma once

#include <Magma/Scene/Scene.h>

namespace Magma {

class SceneLoader {
public:
	static void EditorLoad(Scene& scene, const std::string& path);
	static void EditorSave(const Scene& scene, const std::string& path);
	static void RuntimeSave(const Scene& scene,
							const std::string& projectPath,
							const std::string& exportPath);
};

}