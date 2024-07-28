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
			return CreateRef<OpenGL::Mesh>(vertices, indices, material);
			break;
	}
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, Material material) {
	switch(primitive) {
		case MeshPrimitive::Cube:
			std::vector<Vertex> vertices =
			{
				{ { -0.5, -0.5, -0.5 }, {  0.0,  0.0, -1.0 }, { 0.0, 0.0 } },
				{ {  0.5, -0.5, -0.5 }, {  0.0,  0.0, -1.0 }, { 1.0, 0.0 } },
				{ {  0.5,  0.5, -0.5 }, {  0.0,  0.0, -1.0 }, { 1.0, 1.0 } },
				{ { -0.5,  0.5, -0.5 }, {  0.0,  0.0, -1.0 }, { 0.0, 1.0 } },
				{ { -0.5, -0.5,  0.5 }, {  0.0,  0.0,  1.0 }, { 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, {  0.0,  0.0,  1.0 }, { 1.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, {  0.0,  0.0,  1.0 }, { 1.0, 1.0 } },
				{ { -0.5,  0.5,  0.5 }, {  0.0,  0.0,  1.0 }, { 0.0, 1.0 } },

				{ { -0.5,  0.5, -0.5 }, { -1.0,  0.0,  0.0 }, { 0.0, 0.0 } },
				{ { -0.5, -0.5, -0.5 }, { -1.0,  0.0,  0.0 }, { 1.0, 0.0 } },
				{ { -0.5, -0.5,  0.5 }, { -1.0,  0.0,  0.0 }, { 1.0, 1.0 } },
				{ { -0.5,  0.5,  0.5 }, { -1.0,  0.0,  0.0 }, { 0.0, 1.0 } },
				{ {  0.5, -0.5, -0.5 }, {  1.0,  0.0,  0.0 }, { 0.0, 0.0 } },
				{ {  0.5,  0.5, -0.5 }, {  1.0,  0.0,  0.0 }, { 1.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, {  1.0,  0.0,  0.0 }, { 1.0, 1.0 } },
				{ {  0.5, -0.5,  0.5 }, {  1.0,  0.0,  0.0 }, { 0.0, 1.0 } },

				{ { -0.5, -0.5, -0.5 }, {  0.0, -1.0,  0.0 }, { 0.0, 0.0 } },
				{ {  0.5, -0.5, -0.5 }, {  0.0, -1.0,  0.0 }, { 1.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, {  0.0, -1.0,  0.0 }, { 1.0, 1.0 } },
				{ { -0.5, -0.5,  0.5 }, {  0.0, -1.0,  0.0 }, { 0.0, 1.0 } },
				{ {  0.5,  0.5, -0.5 }, {  0.0,  1.0,  0.0 }, { 0.0, 0.0 } },
				{ { -0.5,  0.5, -0.5 }, {  0.0,  1.0,  0.0 }, { 1.0, 0.0 } },
				{ { -0.5,  0.5,  0.5 }, {  0.0,  1.0,  0.0 }, { 1.0, 1.0 } },
				{ {  0.5,  0.5,  0.5 }, {  0.0,  1.0,  0.0 }, { 0.0, 1.0 } },
			};
			// index data
			std::vector<uint32_t> indices =
			{
				0,  3,  2, // Front
				2,  1,  0,

				4,  5,  6, // Back
				6,  7, 4,

				11,  8,  9, // Left
				9, 10, 11,

				12, 13, 14, // Right
				14, 15, 12,

				16, 17, 18, // Bottom
				18, 19, 16,

				20, 21, 22, // Top
				22, 23, 20
			};
			return Create(vertices, indices, material);

		case MeshPrimitive::Quad:
			std::vector<Vertex> vertices =
			{
				{ { -0.5,  0.5,  0.5 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 } },
				{ {  0.5,  0.5,  0.5 }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 } },
				{ { -0.5, -0.5,  0.5 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 } },
			};
			std::vector<uint32_t> indices =
			{
				0, 2, 3,
				3, 1, 0,
			};

			return Create(vertices, indices, material);
		
		case MeshPrimitive::Line:
			break;
	}
}

}