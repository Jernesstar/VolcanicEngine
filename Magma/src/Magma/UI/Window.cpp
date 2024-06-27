#include "Window.h"

using namespace VolcaniCore;

namespace Magma::UI {


Window::Window(uint32_t width, uint32_t height, const glm::vec4& bgColor,
			   uint32_t borderWidth, uint32_t borderHeight,
			   const glm::vec4& borderColor)
	: UIElement(UIType::Window, width, height),
		m_BackgroundColor(bgColor), m_BorderColor(borderColor),
		m_BorderWidth(borderWidth), m_BorderHeight(borderHeight)
{
	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[](ApplicationUpdatedEvent&) {
		// After the app has finished rendering, we can draw the UI
		ImGui::End();
	});
}

void Window::Draw() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
						ImVec2(m_BorderWidth, m_BorderHeight));

	ImGui::PushStyleColor(ImGuiCol_WindowBg,
	ImVec4(
		m_BackgroundColor.r,
		m_BackgroundColor.g,
		m_BackgroundColor.b,
		m_BackgroundColor.a
	));

	ImGuiWindowFlags windowFlags;
	windowFlags |= ImGuiWindowFlags_None
				| ImGuiWindowFlags_NoDocking
				| ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoBringToFrontOnFocus
				| ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("##Window", &m_Open, windowFlags);

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleVar(3);

	// Not calling ImGUI::End() as the children still need to be rendered
}

bool Window::OnAttach() {
	return false; // For now; more complex docking behavior to come
}

// TODO: Reposition the element within the window
bool Window::OnAddElement(Ref<UIElement> element) {
	return true;
}

}