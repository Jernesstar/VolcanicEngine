#include "UI.h"

#include <imgui/imgui.h>

#include <Core/Log.h>
#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

Button::Button(uint32_t width, uint32_t height, const glm::vec4& color,
	Ref<UIText> uiText)
	: UIElement(UIType::Button, nullptr, width, height), m_Color(color)
{
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&](MouseButtonPressedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width
			&& y <= event.y && event.y <= y - m_Height)
		{
			m_Pressed = true;
			m_Released = false;
			event.Handled = true;

			OnPressed();
		}
	});
	EventSystem::RegisterListener<MouseButtonReleasedEvent>(
	[&](MouseButtonReleasedEvent& event) {
		if(x <= event.x && event.x <= x + m_Width && y <= event.y
			&& event.y <= y - m_Height)
		{
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

void Button::Draw() {
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_Color,
	Transform{
		.Translation = {  x, 0.0f, 0.0f }, .Scale = { m_Width, m_Height, 1.0f }
	});
}

bool Button::OnAttach() {
	return true;
}

bool Button::OnAddElement(Ref<UIElement> element) {
	if(element->Type == UIType::Text)
		return true;
	return false;
}

}