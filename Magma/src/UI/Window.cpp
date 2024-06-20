#include "UI.h"

#include <imgui/imgui.h>

#include <Core/Log.h>
#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {
	
Window(uint32_t width, uint32_t height,const glm::vec4& bgColor,
	const glm::vec4 borderColor, const uint32_t borderWidth,
	const uint32_t borderHeight)
	: UIElement(UIType::Window, nullptr, width, height),
	m_BackgroundColor(bgColor), m_BorderColor(borderColor),
	m_BorderWidth(borderWidth), m_BorderHeight(borderHeight)
{

}

void Window::Draw() {
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(
		m_BackgroundColor, Transform{ .Scale = { m_Width, m_Height, 1.0f } });
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor,
	Transform{
		.Translation = { (m_Width + m_BorderWidth)/2.0f, 0.0f, 0.0f },
		.Scale = { m_BorderWidth, m_Height, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor,
	Transform{
		.Translation = { -float(m_Width + m_BorderWidth)/2.0f, 0.0f, 0.0f },
		.Scale = { m_BorderWidth, m_Height, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor,
	Transform{
		.Translation = { 0.0f, (m_Height + m_BorderHeight)/2.0f, 0.0f },
		.Scale = { m_Width, m_BorderHeight, 1.0f }
	});
	Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_BorderColor,
	Transform{
		.Translation = { 0.0f, -float(m_Height + m_BorderHeight)/2.0f, 0.0f },
		.Scale = { m_Width, m_BorderHeight, 1.0f }
	});
}

bool Window::OnAttach() {
	return false;
}

bool Window::OnAddElement(Ref<UIElement> element) {
	return true;
}

}