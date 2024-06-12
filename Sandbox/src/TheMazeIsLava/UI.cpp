#include "UI.h"


#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {


// UIElement::UIElement() {

// }

// void UIElement::Draw() {

// }

// bool UIElement::OnAttach() {

// }

// bool UIElement::OnAddElement(Ref<UIElement> element) {

// }


void UIWindow::Draw() {
	Renderer::Render2DQuad(m_BackgroundColor, Transform{ .Scale = { m_Width, m_Height, 1.0f } });
	Renderer::Render2DQuad(m_BorderColor, Transform{
		.Translation = {  m_Height, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::Render2DQuad(m_BorderColor, Transform{
		.Translation = { -m_Height, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::Render2DQuad(m_BorderColor, Transform{
		.Translation = {  m_Width, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::Render2DQuad(m_BorderColor, Transform{
		.Translation = { -m_Width, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
}


UIText::UIText()
	: UIElement(UIType::Text)
{

}

void UIText::Draw() {
	Renderer::Draw2DText(m_Text);
}


UIButton::UIButton(const glm::vec4& color, const std::string& text, const glm::vec4& textColor)
	: UIElement(UIType::Button), m_Color(color), m_Text(text), m_TextColor(textColor)
{
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&](MouseButtonPressedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y && event.y <= y - m_Height) {
			m_Pressed = true;
			m_Released = false;
			event.Handled = true;

			OnPressed();
		}
	});
	EventSystem::RegisterListener<MouseButtonReleasedEvent>(
	[&](MouseButtonReleasedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y && event.y <= y - m_Height) {
			m_Pressed = false;
			m_Released = true;
			event.Handled = true;

			OnReleased();
		}
	});
	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[&](const ApplicationUpdatedEvent& event) {
		m_Released = false;
	});
}

void UIButton::Draw() {
	Renderer::Render2DQuad(m_BorderColor, Transform{ 
		.Translation = {  x, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
}

bool UIButton::OnAttach() {
	return true;
}

bool UIButton::OnAddElement(Ref<UIElement> element) {
	if(element.Type == UIType::Text)
		return true;
	return false;
}


UIDropDown::UIDropDown()
	: UIElement(UIType::DropDown)
{
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&](MouseButtonPressedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y && event.y <= y - m_Height) {
			Toggle();
		}
	});
}

void UIDropDown::Draw() {
	if(m_Open) {
		///
		/// Render
		///

		return;
	}
}

bool UIDropDown::OnAttach() {

}

bool UIDropDown::OnAddElement(Ref<UIElement> element) {
	m_Height += element->m_Height;
}


}