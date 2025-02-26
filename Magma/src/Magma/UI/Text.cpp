#include "Text.h"

#include "UIRenderer.h"

namespace Magma::UI {

void Text::Draw() {
	m_State = UIRenderer::DrawText(*this);
	
}

}