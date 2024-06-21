#include "UI.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

DropDown::DropDown()
	: UIElement(Type::DropDown)
{
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&](MouseButtonPressedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width
			&& y <= event.y && event.y <= y - m_Height) {
			Toggle();
		}
	});
}

void DropDown::Draw() {
	if(m_Open) {
		///
		/// Render
		///

		return;
	}
}

bool DropDown::OnAttach() {
	return true;
}

bool DropDown::OnAddElement(Ref<UIElement> element) {
	if(element->Type != UIType::Text)
		return;

	m_Height += element->GetHeight();
	return true;
}

}