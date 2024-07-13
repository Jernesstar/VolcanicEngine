#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Mesh.h"

namespace VolcaniCore {

Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices,
					   const std::vector<uint32_t>& indices,
					   const Material& material)
{
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return std::move(
						CreateRef<OpenGL::Mesh>(vertices, indices, material));
			break;
	}
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, Material material) {
	if(primitive == MeshPrimitive::Cube) {
		std::vector<Vertex> vertices =
		{
			{ { -0.5f,  0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
			{ {  0.5f,  0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
			{ { -0.5f, -0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },

			{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
			{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
			{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		};

		std::vector<uint32_t> indices =
		{
			0, 2, 3, 3, 1, 0,
			5, 7, 6, 6, 4, 5,
			4, 6, 2, 2, 0, 4,
			1, 3, 7, 7, 5, 1,
			4, 0, 1, 1, 5, 4,
			7, 3, 2, 2, 6, 7,
		};

		return Create(vertices, indices, material);
	}
	if(primitive == MeshPrimitive::Quad) {
		std::vector<Vertex> vertices =
		{
			{ { -0.5f,  0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
			{ {  0.5f,  0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
			{ { -0.5f, -0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f,  0.5f }, {  0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
		};
		std::vector<uint32_t> indices =
		{
			0, 2, 3,
			3, 1, 0,
		};

		return Create(vertices, indices, material);
	}
}

}