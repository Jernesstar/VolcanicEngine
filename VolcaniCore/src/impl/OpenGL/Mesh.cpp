#include "Mesh.h"

#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

// TODO(Fix): Remove this
Mesh::Mesh(const std::string& path)
	: VolcaniCore::Mesh(path)
{

}

Mesh::Mesh(const List<Vertex>& verts,
			const List<uint32_t>& indices, const Material& material)
	: VolcaniCore::Mesh(verts, indices, material)
{
}

}