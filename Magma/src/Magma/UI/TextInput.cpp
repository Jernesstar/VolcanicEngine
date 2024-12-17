#include "TextInput.h"

#include "UIRenderer.h"

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIElementType::TextInput),
		MaxCharCount(maxChars), Text("") { }

void TextInput::Draw() {
	m_State = UIRenderer::DrawTextInput(*this);
}

}