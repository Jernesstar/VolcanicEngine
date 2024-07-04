#include "Model.h"

#include "OpenGL/Model.h"

#include "Core/Application.h"

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