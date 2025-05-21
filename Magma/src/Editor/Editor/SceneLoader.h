#pragma once

#include <Magma/Core/YAMLSerializer.h>
#include <Magma/Core/AssetManager.h>
#include <Magma/Script/ScriptObject.h>
#include <Magma/Scene/Scene.h>

namespace Magma {

extern void SaveScript(YAMLSerializer& serializer,
	Ref<Script::ScriptObject> obj);
extern Ref<Script::ScriptObject> LoadScript(Entity entity, Asset asset,
	const std::string& className, YAML::Node& scriptComponentNode);

class SceneLoader {
public:
	static void EditorLoad(Scene& scene, const std::string& path);
	static void EditorSave(const Scene& scene, const std::string& path);
	static void RuntimeSave(const Scene& scene,
							const std::string& projectPath,
							const std::string& exportPath);
};

}