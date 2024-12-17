#include "Window.h"

#include "UIRenderer.h"

namespace Magma::UI {

void Window::Draw() {
	m_State = UIRenderer::DrawWindow(*this);
}

}