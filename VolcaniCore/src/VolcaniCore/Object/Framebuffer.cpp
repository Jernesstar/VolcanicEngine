#include "Framebuffer.h"

#include "OpenGL/Framebuffer.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height) {
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Framebuffer>(width, height);
			break;
	}
}

}