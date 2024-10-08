#pragma once

#include "Object/Mesh.h"

namespace VolcaniCore {

class Model {
public:
	static Ref<Model> Create(const std::string& path);

public:
	const std::string Path;

	Model(const std::string& path);
	~Model() = default;

	std::vector<Ref<Mesh>>::const_iterator begin() const {
		return m_Meshes.begin();
	}
	std::vector<Ref<Mesh>>::const_iterator end() const {
		return m_Meshes.end();
	}

	uint32_t GetMeshCount() const {
		return m_Meshes.size();
	}
	Ref<Mesh> GetMesh(uint32_t i) const {
		return m_Meshes[i];
	}

private:
	void Load(const std::string& path);

	std::vector<Ref<Mesh>> m_Meshes;
};

}