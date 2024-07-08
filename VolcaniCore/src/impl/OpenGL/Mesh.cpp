#include "Mesh.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(std::vector<Vertex> vertices,
		   std::vector<uint32_t> indices,
		   Material material)
	: VolcaniCore::Mesh(vertices, indices, material) { }

Mesh::~Mesh() { }

}