#pragma once

namespace VolcaniCore {

class Mesh {
public:
	Mesh(const std::string& path);
	~Mesh();

	virtual void LoadMesh(const std::string& path);0p

	const std::string& GetPath() const { return m_Path; }

	static Ref<Mesh> Create(const std::string& path);

protected:
	std::string m_Path;
};

}