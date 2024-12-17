#include "Text.h"

#include "UIRenderer.h"

namespace Magma::UI {

Ref<UI::Text> UI::Text::Create(const UI::Text::Specification& specs) {
	return CreateRef<UI::Text>(specs.Text, specs.Color);
}

Text::Text(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIElementType::Text, 0, 0, 0.0f, 0.0f, textColor),
		Content(text) { }

void Text::Draw() {
	m_State = UIRenderer::DrawText(*this);
}

}