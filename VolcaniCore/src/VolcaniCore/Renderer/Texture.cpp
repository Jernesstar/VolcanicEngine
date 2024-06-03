#include "Texture.h"

#include "OpenGL/Texture2D.h"

#include "Core/Application.h"

namespace VolcaniCore {

Ref<Texture> Texture::Create(uint32_t width, uint32_t height) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::Texture2D>(width, height);
			break;
	}
}

Ref<Texture> Texture::Create(const std::string& path) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::Texture2D>(path);
			break;
	}
}

}