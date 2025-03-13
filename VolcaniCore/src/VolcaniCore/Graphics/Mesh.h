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

struct SubMesh {
	Buffer<Vertex> Vertices;
	Buffer<uint32_t> Indices;
	uint32_t MaterialIndex;
};

enum class MeshType { Quad, Cube, Model };

class Mesh {
public:
	static Ref<Mesh> Create(MeshType type,
							Buffer<Vertex>&& vertices,
							Buffer<uint32_t>&& indices,
							const Material& material = { });
	static Ref<Mesh> Create(MeshType type,
							const Material& material = { });
	static Ref<Mesh> Create(MeshType type, const glm::vec4& color);

public:
	const MeshType Type;
	List<SubMesh> SubMeshes;
	List<Material> Materials;

public:
	Mesh(MeshType type)
		: Type(type) { }
	~Mesh() = default;
};

}