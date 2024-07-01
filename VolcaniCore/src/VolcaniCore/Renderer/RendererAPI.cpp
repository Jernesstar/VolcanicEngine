#include "RendererAPI.h"

#include "OpenGL/Renderer.h"

namespace VolcaniCore {


void RendererAPI::Create(RendererBackend backend) {
	switch(backend) {
		case RendererBackend::OpenGL:
			s_Instance = CreateRef<OpenGL::Renderer>();
		// case RenderAPI::Vulkan:
		// 	s_Instance = CreateRef<Vulkan::Renderer>();
	}

	s_Instance->Init();
}


}