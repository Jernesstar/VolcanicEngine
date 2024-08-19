#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Object/Texture.h"

namespace VolcaniCore {

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec4 TexCoord_Color; // if the alpha channel is 0, used as tex coord
};

struct Material {
	Ref<Texture> Diffuse = nullptr;
	Ref<Texture> Specular = nullptr;
};

enum class MeshPrimitive { Point, Line, Quad, Cube, Pyramid };

class Mesh {
public:
	const std::string Path;

public:
	static Ref<Mesh> Create(const std::string& path);

	static Ref<Mesh> Create(const std::vector<Vertex>& vertices,
							const std::vector<uint32_t>& indices,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive, const glm::vec4& color);

public:
	Mesh() = default;
	Mesh(const std::string& path);

	Mesh(const std::vector<Vertex>& vertices,
		 const std::vector<uint32_t>& indices,
		 const Material& material = { })
			: m_Vertices(vertices), m_Indices(indices), m_Material(material),
				Path("") { }

	virtual ~Mesh() = default;

	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
	const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
	Material& GetMaterial() { return m_Material; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Mesh>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	Material m_Material;
};

}