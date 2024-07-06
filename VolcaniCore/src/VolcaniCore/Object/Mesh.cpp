#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Mesh.h"

namespace VolcaniCore {

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

}