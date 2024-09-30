#pragma once

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

class Scene;

class SceneSerializer {
public:
	SceneSerializer() = default;
	SceneSerializer(Scene* scene);
	~SceneSerializer() = default;

	void Serialize(const std::string& path);
	void Deserialize(const std::string& path);

private:
	Scene* m_Scene;
};

}