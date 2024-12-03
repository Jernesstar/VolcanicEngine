#include "Button.h"

#include "UIPage.h"

namespace Magma::UI {

Button::Button(const glm::vec4& color, const std::string& text,
				const glm::vec4& textColor)
	: UIElement(UIElement::Type::Button)
{
	Color = color;
	// Text = text;
}

Button::Button(const std::string& imagePath)
	: UIElement(UIElement::Type::Button)
{
	// Display = CreateRef<Image>(imagePath);
}

void Button::Draw() {
	m_State = UIRenderer::DrawButton(*this);
	if(m_Root)
		m_Root->OnEvent(m_State, m_ID);
}

}