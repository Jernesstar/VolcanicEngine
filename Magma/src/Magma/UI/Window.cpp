#include "Window.h"

#include <imgui/imgui.h>

namespace Magma::UI {

Ref<Window> Window::Create(const Window::Specification& specs) {
	return CreateRef<UI::Window>(
		specs.Name, specs.Width, specs.Height, specs.x, specs.y, specs.Color,
		specs.BorderWidth, specs.BorderHeight, specs.BorderColor
	);
}

Window::Window(std::string name, uint32_t width, uint32_t height,
				float x, float y, const glm::vec4& bgColor,
				uint32_t borderWidth, uint32_t borderHeight,
				const glm::vec4& borderColor)
	: UIElement(UIElement::Type::Window, width, height, x, y, bgColor),
		m_Name(name), m_BorderWidth(borderWidth), m_BorderHeight(borderHeight),
		m_BorderColor(borderColor) { }

void Window::Draw() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + x, viewport->Pos.y + y));
	ImGui::SetNextWindowSize(ImVec2(m_Width, m_Height));
	// ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
						ImVec2(m_BorderWidth, m_BorderHeight));

	ImGui::PushStyleColor(ImGuiCol_WindowBg,
						  ImVec4{ m_Color.r, m_Color.g, m_Color.b, m_Color.a });

	ImGuiWindowFlags windowFlags;
	windowFlags |= ImGuiWindowFlags_NoDocking
				 | ImGuiWindowFlags_NoTitleBar
				 | ImGuiWindowFlags_NoCollapse
				 | ImGuiWindowFlags_NoResize
				 | ImGuiWindowFlags_NoMove
				 | ImGuiWindowFlags_NoBringToFrontOnFocus
				 | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin(m_Name.c_str(), &m_Open, windowFlags);

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(3);

	// Not calling ImGUI::End() as the children still need to be rendered
}

bool Window::OnAttach() {
	return false; // TODO(Implement): More complex docking behavior
}

bool Window::OnAddElement(Ref<UIElement> element) {
	return true;
}

}