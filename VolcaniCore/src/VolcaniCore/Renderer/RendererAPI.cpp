#include "RendererAPI.h"

#include "ShaderLibrary.h"

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

	ShaderLibrary::Init();

	s_Instance->Init();
}

void RendererAPI::Shutdown() {
	s_Instance->Close();
}


}