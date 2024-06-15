#include "UI.h"

#include <Core/Log.h>
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
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BackgroundColor, Transform{ .Scale = { m_Width, m_Height, 1.0f } });
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor, Transform{
		.Translation = { (m_Width + m_BorderWidth)/2.0f, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_Height, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor, Transform{
		.Translation = { -float(m_Width + m_BorderWidth)/2.0f, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_Height, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor, Transform{
		.Translation = { 0.0f, (m_Height + m_BorderHeight)/2.0f, 0.0f }, .Scale = { m_Width, m_BorderHeight, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor, Transform{
		.Translation = { 0.0f, -float(m_Height + m_BorderHeight)/2.0f, 0.0f }, .Scale = { m_Width, m_BorderHeight, 1.0f }
	});
}


UIText::UIText(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIType::Text)
{

}

void UIText::Draw() {
	// Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DText(m_Text);
}


UIButton::UIButton(uint32_t width, uint32_t height, const glm::vec4& color, Ref<UIText> uiText)
	: UIElement(UIType::Button, nullptr, width, height), m_Color(color)
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

	if(uiText != nullptr)
		Add(m_Text = uiText);
}

void UIButton::Draw() {
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_Color, Transform{ 
		.Translation = {  x, 0.0f, 0.0f }, .Scale = { m_Width, m_Height, 1.0f }
	});
}

bool UIButton::OnAddElement(Ref<UIElement> element) {
	if(element->Type == UIType::Text)
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
	return true;
}

bool UIDropDown::OnAddElement(Ref<UIElement> element) {
	m_Height += element->GetHeight();

	return true;
}


}