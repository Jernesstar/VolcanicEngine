#include "UI.h"

#include <imgui/imgui.h>

#include <Core/Log.h>
#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

Text::Text(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIType::Text)
{

}

void Text::Draw() {
	// Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DText(m_Text);
}

bool Text::OnAttach() {
	return true;
}

bool Text::OnAddElement(Ref<UIElement> element) {
	return true;
}

}