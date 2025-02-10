#pragma once

#include "Graphics/Mesh.h"

namespace VolcaniCore {

class Model {
public:
	static Ref<Model> Create(const std::string& path);

public:
	const std::string Path;

	Model(const std::string& path);
	~Model() = default;

	void AddMesh(Ref<Mesh> mesh);

	Ref<Mesh> GetMesh(int64_t i) const {
		return m_Meshes[i];
	}

	uint32_t GetMeshCount() const {
		return m_Meshes.Count();
	}

	List<Ref<Mesh>>::const_iterator begin() const {
		return m_Meshes.begin();
	}
	List<Ref<Mesh>>::const_iterator end() const {
		return m_Meshes.end();
	}

private:
	void Load(const std::string& path);

	List<Ref<Mesh>> m_Meshes;
};

}