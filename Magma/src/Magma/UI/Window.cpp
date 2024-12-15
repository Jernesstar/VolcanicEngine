#include "Window.h"

#include "UIPage.h"

namespace Magma::UI {

void Window::Draw() {
	m_State = UIRenderer::DrawWindow(*this);
}

}