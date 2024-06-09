#include "UI.h"

#include <OpenGL/Renderer.h>

namespace TheMazeIsLava {

// UIElement::UIElement() {

// }

// void UIElement::Draw() override {

// }

// bool UIElement::OnAttach() override {

// }

// bool UIElement::OnAddElement(Ref<UIElement> element) override {

// }

void UIWindow::Draw() override {
	Renderer::DrawQuad(m_BackgroundColor, Transform{ .Scale = { m_Width, m_Height, 1.0f } });
	Renderer::DrawQuad(m_BorderColor, Transform{ 
		.Translation = {  m_Height, 0.0f, 0.0f },.Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::DrawQuad(m_BorderColor, Transform{ 
		.Translation = { -m_Height, 0.0f, 0.0f },.Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::DrawQuad(m_BorderColor, Transform{ 
		.Translation = {  m_Width, 0.0f, 0.0f },.Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::DrawQuad(m_BorderColor, Transform{ 
		.Translation = { -m_Width, 0.0f, 0.0f },.Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
}

bool UIWindow::OnAttach() override {
	
}

bool UIWindow::OnAddElement(Ref<UIElement> element) override {

}

UIButton::UIButton(const std::string& text = "")
	: m_Text(text)
{
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&](MousePressedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y && event.y <= y - m_Height) {
			m_Pressed = true;
			m_Released = false;
			event.Handled = true;
		}
	});
	EventSystem::RegisterListener<MouseButtonReleasedEvent>(
	[&](MouseButtonReleasedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y && event.y <= y - m_Height) {
			m_Pressed = false;
			m_Released = true;
			event.Handled = true;
		}
	});
}

void UIButton::Draw() override {
	Renderer::DrawQuad(m_BorderColor, Transform{ 
		.Translation = {  x, 0.0f, 0.0f }, .Scale = { m_BorderWidth, m_BorderHeight, 1.0f }
	});
	Renderer::DrawText(m_Text, Transform{ .Translation = { x, 0.0f, 0.0f } });
}

bool UIButton::OnAttach() override {

}

UIDropDown::UIDropDown() {

}

void UIDropDown::Draw() override {

}

bool UIDropDown::OnAttach() override {

}

bool UIDropDown::OnAddElement(Ref<UIElement> element) override {

}

}