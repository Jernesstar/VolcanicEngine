#include "Texture.h"

#include "OpenGL/Texture2D.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

Ref<Texture> Texture::Create(uint32_t width, uint32_t height) {
	RendererAPI::Backend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(width, height);
			break;
	}
}

Ref<Texture> Texture::Create(const std::string& path) {
	RendererAPI::Backend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(path);
			break;
	}
}

}