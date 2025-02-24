#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Core/Defines.h"
#include "Core/List.h"

#include "Graphics/Texture.h"

namespace VolcaniCore {

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct Material {
	Ref<Texture> Diffuse = nullptr;
	Ref<Texture> Specular = nullptr;
	Ref<Texture> Emissive = nullptr;

	glm::vec4 DiffuseColor = glm::vec4(0.0f);
	glm::vec4 SpecularColor = glm::vec4(0.0f);
	glm::vec4 EmissiveColor = glm::vec4(0.0f);
};

enum class MeshPrimitive { Quad, Cube, Pyramid, Sphere };

struct SubMesh {
	List<Vertex> Vertices;
	List<uint32_t> Indices;
	Material Mat;
};

class Mesh {
public:
	static Ref<Mesh> Create(const List<Vertex>& vertices,
							const List<uint32_t>& indices,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive,
							const Material& material = { });

	static Ref<Mesh> Create(MeshPrimitive primitive, const glm::vec4& color);

public:
	List<SubMesh> SubMeshes;

public:
	Mesh() = default;
	~Mesh() = default;
};

}