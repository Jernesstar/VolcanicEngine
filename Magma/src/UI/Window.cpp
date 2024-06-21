#include "UI.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

static ImGuiWindowFlags windowFlags;

Window(uint32_t width, uint32_t height,const glm::vec4& bgColor,
		const glm::vec4 borderColor, const uint32_t borderWidth,
		const uint32_t borderHeight)
	: UIElement(UIType::Window, width, height),
		m_BackgroundColor(bgColor), m_BorderColor(borderColor),
		m_BorderWidth(borderWidth), m_BorderHeight(borderHeight)
{
	windowFlags |= ImGuiWindowFlags_None
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus;

	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[](ApplicationUpdatedEvent&) {
		ImGUI::End();
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
		m_BackgroundColor.a)
	);

	ImGUI::Begin("###Window", &m_Open, windowFlags);

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleVar(3);
}

bool Window::OnAttach() {
	return false;
}

bool Window::OnAddElement(Ref<UIElement> element) {
	return true;
}

}