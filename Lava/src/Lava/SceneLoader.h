#pragma once

#include <Magma/Scene/Scene.h>

using namespace Magma;

namespace Lava {

class SceneLoader {
public:
	static void EditorLoad(Scene& scene, const std::string& path);
	static void EditorSave(const Scene& scene, const std::string& path);
	static void RuntimeSave(const Scene& scene, const std::string& scenePack);
	static void RuntimeLoad(Scene& scene, const std::string& scenePack, const std::string& section);
};

}