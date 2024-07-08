#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Mesh.h"

namespace VolcaniCore {

static std::vector<Vertex> CubeVertices;
// static std::vector<Vertex> QuadVertices;

static std::vector<uint32_t> CubeIndices;
static std::vector<uint32_t> QuadIndices;

Ref<Mesh> Mesh::Create(std::vector<Vertex> vertices,
					   std::vector<uint32_t> indices,
					   Material material)
{
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Mesh>(vertices, indices, material);
			break;
	}
}

Ref<Mesh> Create(MeshPrimitive primitive, Material material) {
	switch(primitive) {
		case MeshPrimitive::Cube:
			return Create(CubeVertices, CubeIndices, material);
		case MeshPrimitive::Quad:
			return Create(CubeVertices, QuadIndices, material);
	}
}

CubeVertices =
{
	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },	// 0 Front Top Left
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },	// 1 Front Top Right
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },	// 2 Front Bottom Left
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },	// 3 Front Bottom Right

	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },	// 4 Back Top Left
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },	// 5 Back Top Right
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },	// 6 Back Bottom Left
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },	// 7 Back Bottom Right
};

CubeIndices =
{
	0, 2, 3,
	3, 1, 0,

	5, 7, 6,
	6, 4, 5,

	4, 6, 2,
	2, 0, 4,

	1, 3, 7,
	7, 5, 1,

	4, 0, 1,
	1, 5, 4,

	7, 3, 2,
	2, 6, 7,
};

QuadIndices =
{
	0, 2, 3,
	3, 1, 0
};


}