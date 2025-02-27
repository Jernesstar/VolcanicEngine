#include "Cubemap.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/Cubemap.h"

namespace VolcaniCore {

Ref<Cubemap> Cubemap::Create(const List<ImageData>& faces) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Cubemap>(faces);
	}

	return nullptr;
}

}