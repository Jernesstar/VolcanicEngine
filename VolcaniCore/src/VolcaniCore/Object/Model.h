#pragma once

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Object/Texture.h"

namespace VolcaniCore {

class Model {
public:
	static Ref<Model> Create(const std::string& path);

	std::vector<Ref<Mesh>>::const_iterator begin() const {
		return m_Meshes.begin();
	}
	std::vector<Ref<Mesh>>::const_iterator end() const {
		return m_Meshes.end();
	}

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	std::vector<Ref<Mesh>> m_Meshes;
};

}