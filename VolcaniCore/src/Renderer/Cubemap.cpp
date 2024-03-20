#include "Renderer/Cubemap.h"
#include "Renderer/Renderer.h"

namespace VolcaniCore {

Ref<Cubemap> Cubemap::Create(const std::string& cubemap_folder) {
	RenderAPI api = Renderer::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<VolcaniCore::OpenGL::Cubemap>(cubemap_folder);
			break;
	}
}

Ref<Cubemap> Cubemap::Create(const std::vector<std::string>& faces) {


}

}