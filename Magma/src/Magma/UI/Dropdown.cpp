#include "Dropdown.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

DropDown::DropDown()
	: UIElement(UIType::DropDown)
{

}

void DropDown::Draw() {
	uint32_t n = m_Children.size();
	const char* items[n];
	for(uint32_t i = 0; i < n; i++)
		items[i] = m_Children[i]->As<Text>()->GetText().c_str(); // TODO: Images ?
	if(ImGui::BeginCombo("##Combo", m_CurrentItem))
	{
		for(int i = 0; i < n; i++) {
			bool isSelected = (m_CurrentItem == items[i]);
			if(ImGui::Selectable(items[i], isSelected))
				m_CurrentItem = items[i];
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

bool DropDown::OnAttach() {
	return true;
}

bool DropDown::OnAddElement(Ref<UIElement> element) {
	return true;
}

}