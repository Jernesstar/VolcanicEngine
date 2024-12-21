#include "Button.h"

#include "UIRenderer.h"

#include "Text.h"
#include "Image.h"

namespace Magma::UI {

Button::Button(const glm::vec4& color, const std::string& text,
				const glm::vec4& textColor)
	: UIElement(UIElementType::Button)
{
	Color = color;
	Display = CreateRef<Text>(text, textColor);
}

Button::Button(const std::string& imagePath)
	: UIElement(UIElementType::Button)
{
	Display = CreateRef<Image>(imagePath);
}

void Button::Draw() {
	m_State = UIRenderer::DrawButton(*this);
}

}