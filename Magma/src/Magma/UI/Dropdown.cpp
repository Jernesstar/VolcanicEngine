#include "Dropdown.h"

#include <imgui/imgui.h>

#include "UIRenderer.h"

using namespace VolcaniCore;

namespace Magma::UI {

Dropdown::Dropdown()
	: UIElement(UIElementType::Dropdown) { }

void Dropdown::Draw() {
	m_State = UIRenderer::DrawDropdown(*this);
}

}