#include "Mesh.h"

namespace VolcaniCore {

Ref<Mesh> Mesh::Create(MeshType type,
						const List<Vertex>& vertices,
						const List<uint32_t>& indices,
						const Material& material)
{
	auto mesh = CreateRef<Mesh>(type);
	mesh->SubMeshes.Emplace(vertices, indices, 0U);
	mesh->Materials.Add(material);
	return mesh;
}

Ref<Mesh> Mesh::Create(MeshType type, const Material& material) {
	List<uint32_t> indices;
	List<Vertex> vertices;

	switch(type) {
		case MeshType::Cube:
		{
			vertices =
			{
				{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
				{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
				{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
				{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
				{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
				{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
				{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
				{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },

				{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
				{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
				{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
				{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
				{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
				{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
				{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
				{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },

				{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } },
				{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
				{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } },
				{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },
				{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
				{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
				{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },
				{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
			};

			indices =
			{
				0,  3,  2, // Front
				2,  1,  0,

				4,  5,  6, // Back
				6,  7,  4,

				11,  8,  9, // Left
				 9, 10, 11,

				12, 13, 14, // Right
				14, 15, 12,

				16, 17, 18, // Bottom
				18, 19, 16,

				20, 21, 22, // Top
				22, 23, 20
			};

			break;
		}

		case MeshType::Quad:
		{
			vertices =
			{
				{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
				{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
				{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
				{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
			};

			indices =
			{
				0, 2, 3,
				3, 1, 0,
			};

			break;
		}
	}

	return Create(type, vertices, indices, material);
}

Ref<Mesh> Mesh::Create(MeshType type, const glm::vec4& color) {
	return Create(type, Material{ .DiffuseColor = color });
}

}