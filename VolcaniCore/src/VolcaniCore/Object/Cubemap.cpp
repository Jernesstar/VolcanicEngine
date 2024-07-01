#include "Cubemap.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/Cubemap.h"

namespace VolcaniCore {

Ref<Cubemap> Cubemap::Create(const std::string& cubemap_folder) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Cubemap>(cubemap_folder);
			break;
	}
}

Ref<Cubemap> Cubemap::Create(const std::vector<std::string>& faces) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Cubemap>(faces);
			break;
	}
}

}