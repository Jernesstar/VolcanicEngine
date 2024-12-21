#include "Texture.h"

#include "OpenGL/Texture2D.h"

#include "Graphics/RendererAPI.h"

namespace VolcaniCore {

Ref<Texture> Texture::Create(uint32_t width, uint32_t height,
							 Format format, Sampling sampling)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(width, height, format, sampling);
			break;
	}
}

Ref<Texture> Texture::Create(const std::string& path) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(path);
			break;
	}
}

}