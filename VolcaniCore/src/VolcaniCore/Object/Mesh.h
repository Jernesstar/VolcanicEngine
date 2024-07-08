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
	static Ref<Mesh> Create(std::vector<Vertex> vertices,
							std::vector<uint32_t> indices,
							Material material);
	static Ref<Mesh> Create(MeshPrimitive primitive, Material material);

public:
	const std::vector<Vertex> Vertices;
	const std::vector<uint32_t> Indices;
	const Material Material;

	Mesh(std::vector<Vertex> vertices,
		 std::vector<uint32_t> indices,
		 Material material)
			: Vertices(vertices), Indices(indices), Material(material) { }

	~Mesh() {
		Vertices.clear();
		Indices.clear();
	}

	Material& GetMaterial() { return Material; }

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}