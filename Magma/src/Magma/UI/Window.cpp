#include "Window.h"

#include "UIPage.h"

namespace Magma::UI {

void Window::Draw() {
	m_State = UIRenderer::DrawWindow(*this);
	if(m_Root)
		m_Root->OnEvent(m_State, m_ID);
}

}