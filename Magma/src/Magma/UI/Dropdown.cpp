#include "Dropdown.h"

#include <imgui/imgui.h>

#include "Text.h"

using namespace VolcaniCore;

namespace Magma::UI {

Dropdown::Dropdown()
	: UIElement(UIElement::Type::Dropdown) { }

void Dropdown::Draw() {
	uint32_t n = m_Children.size();
	//const char* items[n];
	const char** items = new const char*[255];
	// TODO(Implement): Images
	for(uint32_t i = 0; i < n; i++)
		items[i] = m_Children[i]->As<UI::Text>()->Content.c_str();

	if(ImGui::BeginCombo("##Combo", m_CurrentItem))
	{
		for(uint32_t i = 0; i < n; i++) {
			bool isSelected = (m_CurrentItem == items[i]);
			if(ImGui::Selectable(items[i], isSelected))
				m_CurrentItem = items[i];
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

bool Dropdown::OnAttach() {
	return true;
}

bool Dropdown::OnAddElement(Ref<UIElement> element) {
	if(element->GetType() == UIElement::Type::Text)
		return true;
	return false;
}

}