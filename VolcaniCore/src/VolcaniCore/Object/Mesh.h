#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Object/Texture.h"

namespace VolcaniCore {

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TextureCoordinate;
};

struct Material {
	Ref<Texture> Diffuse;
	Ref<Texture> Specular;
	Ref<Texture> Roughness;
};

class Mesh {
public:
	static Ref<Mesh> Create(std::vector<Vertex> vertices,
							std::vector<uint32_t> indices,
							Material material);

public:
	Mesh(std::vector<Vertex> vertices,
		 std::vector<uint32_t> indices,
		 Material material)
			: m_Vertices(vertices), m_Indices(indices), m_Material(material) { }

	~Mesh() {
		m_Vertices.clear();
		m_Indices.clear();
	}

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	Material m_Material;
};

}