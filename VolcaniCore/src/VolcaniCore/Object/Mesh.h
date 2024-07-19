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

enum class MeshPrimitive { Line, Quad, Cube, Pyramid };

class Mesh {
public:
	static Ref<Mesh> Create(const std::vector<Vertex>& vertices,
							const std::vector<uint32_t>& indices,
							const Material& material);
	static Ref<Mesh> Create(MeshPrimitive primitive, Material material);

public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices,
		 const std::vector<uint32_t>& indices,
		 const Material& material)
			: m_Vertices(vertices), m_Indices(indices), m_Material(material) { }

	virtual ~Mesh() = default;
	Mesh(Mesh&&) = default;
	Mesh& operator =(const Mesh&) = default;

	Material& GetMaterial() { return m_Material; }
	std::vector<Vertex>& GetVertices() { return m_Vertices; }

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	Material m_Material;
};

}