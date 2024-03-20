#include "Renderer.h"

#include "OpenGL/Renderer.h"

namespace VolcaniCore {

void Renderer::Init(RenderAPI api) {
	if(api == RenderAPI::OpenGL)
		m_BackendRenderer = CreateRef<OpenGL::Renderer>();

	m_BackendRenderer->Init();
}

void Renderer::Close() {
	m_BackendRenderer->Close();
}

}