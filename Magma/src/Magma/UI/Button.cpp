#include "Button.h"

#include "UIRenderer.h"

#include "Text.h"
#include "Image.h"

namespace Magma::UI {

void Button::Draw() {
	m_State = UIRenderer::DrawButton(*this);
}

}