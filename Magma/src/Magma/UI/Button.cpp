#include "Button.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <OpenGL/Texture2D.h>

namespace Magma::UI {

Button::Button(const glm::vec4& color, const std::string& text,
				const glm::vec4& textColor)
	: UIElement(UIElement::Type::Button)
{
	// Add(CreateRef<UI::Text>(text, textColor));
	Color = color;
}

Button::Button(const std::string& imagePath)
	: UIElement(UIElement::Type::Button)
{
	// Add(CreateRef<UI::Image>(imagePath));
}

}