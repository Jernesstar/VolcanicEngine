#include "TextInput.h"

#include "UIPage.h"

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIElement::Type::TextInput),
		MaxCharCount(maxChars), Text("") { }

void TextInput::Draw() {
	m_State = UIRenderer::DrawTextInput(*this);
	if(m_Root)
		m_Root->OnEvent(m_ID, m_State);
}

}