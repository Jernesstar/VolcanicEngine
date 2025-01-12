#include "AssetEditorPanel.h"

#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

namespace Magma {

AssetEditorPanel::AssetEditorPanel(const std::string& path)
	: Panel("AssetEditor"), m_Path(path) { }

void AssetEditorPanel::Update(TimeStep ts) {

}

static bool HasExtension(fs::path path, const List<std::string>& extensions) {
	for(const auto& ext : extensions)
		if(path.extension() == ext)
			return true;

	return false;
}

void AssetEditorPanel::Draw() {
	ImGui::Begin("Asset Editor", &m_Open);
	{

	}
	ImGui::End();
}

}