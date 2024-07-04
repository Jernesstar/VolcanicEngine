#include "Framebuffer.h"

#include "OpenGL/Framebuffer.h"

#include "Core/Application.h"

namespace VolcaniCore {

Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Framebuffer>(width, height);
			break;
	}
}

}