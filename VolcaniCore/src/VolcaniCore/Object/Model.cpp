#include "Model.h"

#include "OpenGL/Model.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

Ref<Model> Model::Create(const std::string& path) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Model>(path);
			break;
	}
}

}