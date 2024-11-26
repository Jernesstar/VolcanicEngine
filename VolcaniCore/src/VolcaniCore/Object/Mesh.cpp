#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Mesh.h"

#include "Model.h"

namespace VolcaniCore {

Mesh::Mesh(const std::string& path)
	: Path(path)
{
	Ref<Mesh> mesh = Model::Create(path)->GetMesh(0);
	this->m_Vertices = mesh->GetVertices();
	this->m_Indices  = mesh->GetIndices();
	this->m_Material = mesh->GetMaterial();
}

Ref<Mesh> Mesh::Create(const std::string& path) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Mesh>(path);
			break;
	}
}

Ref<Mesh> Mesh::Create(const List<Vertex>& vertices,
						const List<uint32_t>& indices,
						const Material& material)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Mesh>(vertices, indices, material);
			break;
	}
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, const Material& material) {
	List<uint32_t> indices;
	List<Vertex> vertices;

	switch(primitive) {
		case MeshPrimitive::Cube:
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

		case MeshPrimitive::Quad:
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

	return Create(vertices, indices, material);
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, const glm::vec4& color) {
	return Create(primitive, Material{ .DiffuseColor = color });
}

}