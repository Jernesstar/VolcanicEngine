#include "Cubemap.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/Cubemap.h"

namespace VolcaniCore {

Ref<Cubemap> Cubemap::Create(const List<Buffer<uint8_t>>& faces) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Cubemap>(faces);
	}

	return nullptr;
}

}