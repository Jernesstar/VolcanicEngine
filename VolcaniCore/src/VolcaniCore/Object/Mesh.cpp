#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Mesh.h"

namespace VolcaniCore {

Ref<Mesh> Mesh::Create() {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Mesh>();
			break;
	}
}

}