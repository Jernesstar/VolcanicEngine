#include "AssetPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

AssetPanel::AssetPanel(const std::string& path)
	: Panel("Asset"), m_Path(path)
{
}

void AssetPanel::Update(TimeStep ts) {

}

void AssetPanel::Draw() {
	ImGui::Begin("Assets", &m_Open);
	{

	}
	ImGui::End();
}

}