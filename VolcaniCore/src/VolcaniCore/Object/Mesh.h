#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Core/Defines.h"

#include "Object/Texture.h"
#include "Object/Vertex.h"

namespace VolcaniCore {

struct Material {
	Ref<Texture> Diffuse = nullptr;
	Ref<Texture> Specular = nullptr;
	Ref<Texture> Emissive = nullptr;

	glm::vec4 DiffuseColor = glm::vec4(0.0f);
	glm::vec4 SpecularColor = glm::vec4(0.0f);
	glm::vec4 EmissiveColor = glm::vec4(0.0f);
};

enum class MeshPrimitive { Point, Line, Quad, Cube, Pyramid };

class Mesh {
public:
	const std::string Path;

public:
	static Ref<Mesh> Create(const std::string& path);

	static Ref<Mesh> Create(const List<Vertex>& vertices,
							const List<uint32_t>& indices,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive, const glm::vec4& color);

public:
	Mesh() = default;
	Mesh(const std::string& path);

	Mesh(const List<Vertex>& vertices,
		 const List<uint32_t>& indices,
		 const Material& material = { })
			: m_Vertices(vertices), m_Indices(indices), m_Material(material),
				Path("") { }

	virtual ~Mesh() = default;

	const List<Vertex>& GetVertices() const { return m_Vertices; }
	const List<uint32_t>& GetIndices() const { return m_Indices; }
	Material& GetMaterial() { return m_Material; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Mesh>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	List<Vertex> m_Vertices;
	List<uint32_t> m_Indices;
	Material m_Material;
};

}