#include "Renderer.h"

#include "OpenGLRenderer.h"

namespace VolcaniCore {

void Renderer::Init(RenderAPI api) {
	if(api == RenderAPI::OpenGL)
		m_BackendRenderer = CreateRef<OpenGLRenderer>();

	m_BackendRenderer->Init();
}
}