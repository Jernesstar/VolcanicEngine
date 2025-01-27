#include "UIElementEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UI.h>

using namespace Magma::UI;

namespace Magma {

UIElementEditorPanel::UIElementEditorPanel(UI::UIPage* page)
	: Panel("UIElementEditor")
{
	SetContext(page);
}

void UIElementEditorPanel::SetContext(UI::UIPage* page) {
	m_Context = page;
}

void UIElementEditorPanel::Update(TimeStep ts) {

}

template<typename TUIElement>
static void EditElement(UIElement* element);

template<>
void EditElement<Window>(UIElement* element) {
	
}

template<>
void EditElement<Button>(UIElement* element) {
	
}

template<>
void EditElement<Image>(UIElement* element) {

}

template<>
void EditElement<Text>(UIElement* element) {

}

template<>
void EditElement<TextInput>(UIElement* element) {

}

template<>
void EditElement<Dropdown>(UIElement* element) {

}

void UIElementEditorPanel::Draw() {
	ImGui::Begin("UI Element Editor", &m_Open);
	{
		if(m_Selected) {
			switch(m_Selected->GetType()) {
				case UIElementType::Window:
					EditElement<Window>(m_Selected);
					break;
				case UIElementType::Button:
					EditElement<Button>(m_Selected);
					break;
				case UIElementType::Image:
					EditElement<Image>(m_Selected);
					break;
				case UIElementType::Text:
					EditElement<Text>(m_Selected);
					break;
				case UIElementType::TextInput:
					EditElement<TextInput>(m_Selected);
					break;
				case UIElementType::Dropdown:
					EditElement<Dropdown>(m_Selected);
					break;
			}
		}
	}
	ImGui::End();
}

}