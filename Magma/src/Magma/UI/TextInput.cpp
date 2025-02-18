#include "TextInput.h"

#include "UIRenderer.h"

namespace Magma::UI {

void TextInput::Draw() {
	m_State = UIRenderer::DrawTextInput(*this);
}

}