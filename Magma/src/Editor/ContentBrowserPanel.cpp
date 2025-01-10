#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace Magma {

ContentBrowserPanel::ContentBrowserPanel(const std::string& path)
	: Panel("ContentBrowser"), m_Path(path) { }

void ContentBrowserPanel::Update(TimeStep ts) {

}

void ContentBrowserPanel::Render() {
	ImGui::Begin("Assets", &m_Open);
	{

	}
	ImGui::End();
}

}