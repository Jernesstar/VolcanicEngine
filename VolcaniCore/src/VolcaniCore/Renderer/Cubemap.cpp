#include "Cubemap.h"

#include "Renderer.h"

#include "OpenGL/Cubemap.h"

namespace VolcaniCore {

Ref<Cubemap> Cubemap::Create(const std::string& cubemap_folder) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::Cubemap>(cubemap_folder);
			break;
	}
}

Ref<Cubemap> Cubemap::Create(const std::vector<std::string>& faces) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::Cubemap>(faces);
			break;
	}
}

}