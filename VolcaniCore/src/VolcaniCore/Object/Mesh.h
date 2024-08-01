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
	// TODO: glm::vec4 TextureCoordinate_Color;
};

struct Material {
	Ref<Texture> Diffuse;
	Ref<Texture> Specular;
	Ref<Texture> Roughness;
};

enum class MeshPrimitive { Point, Line, Quad, Cube, Pyramid };

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

	std::vector<Vertex>& GetVertices() { return m_Vertices; }
	std::vector<uint32_t>& GetIndices() { return m_Indices; }
	Material& GetMaterial() { return m_Material; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Mesh>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	// TODO: Replace vectors with Buffer<T> type
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	Material m_Material;
};

}