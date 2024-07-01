#include "Texture.h"

#include "OpenGL/Texture2D.h"

#include "Core/Application.h"

namespace VolcaniCore {

Ref<Texture> Texture::Create(uint32_t width, uint32_t height) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(width, height);
			break;
	}
}

Ref<Texture> Texture::Create(const std::string& path) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::Texture2D>(path);
			break;
	}
}

}