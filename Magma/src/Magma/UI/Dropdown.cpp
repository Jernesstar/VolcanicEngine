#include "Dropdown.h"

#include <imgui/imgui.h>

#include "UIPage.h"

using namespace VolcaniCore;

namespace Magma::UI {

Dropdown::Dropdown()
	: UIElement(UIElement::Type::Dropdown) { }

void Dropdown::Draw() {
	m_State = UIRenderer::DrawDropdown(*this);
}

}