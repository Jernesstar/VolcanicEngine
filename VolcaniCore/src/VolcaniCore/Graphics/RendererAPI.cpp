#include "RendererAPI.h"

#include "OpenGL/Renderer.h"

namespace VolcaniCore {


void RendererAPI::Create(RendererAPI::Backend backend) {
	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			s_Instance = CreateRef<OpenGL::Renderer>();
			break;
		// case RenderAPI::Vulkan:
		// 	s_Instance = CreateRef<Vulkan::Renderer>();
		// 	break;
	}

	s_Instance->Init();
}

void RendererAPI::Shutdown() {
	s_Instance->Close();
}


}