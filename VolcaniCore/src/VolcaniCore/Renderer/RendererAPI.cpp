#include "RendererAPI.h"

#include "ShaderLibrary.h"

#include "OpenGL/Renderer.h"

namespace VolcaniCore {


void RendererAPI::Create(RendererBackend backend) {
	switch(backend) {
		case RendererBackend::OpenGL:
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