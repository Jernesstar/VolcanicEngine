#include "AssetEditorPanel.h"

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

namespace Magma {

AssetEditorPanel::AssetEditorPanel()
	: Panel("AssetEditor") { }

void AssetEditorPanel::Update(TimeStep ts) {

}

static bool HasExtension(fs::path path, const List<std::string>& extensions) {
	for(const auto& ext : extensions)
		if(path.extension() == ext)
			return true;

	return false;
}

static void EditAsset(Asset asset) {
	ImGui::SeparatorText("Asset");
	ImGui::Indent(22.0f);

	std::string type;
	switch(asset.Type) {
		case AssetType::Mesh:
			type = "Mesh";
			break;
		case AssetType::Texture:
			type = "Texture";
			break;
		case AssetType::Cubemap:
			type = "Cubemap";
			break;
		case AssetType::Font:
			type = "Font";
			break;
		case AssetType::Sound:
			type = "Sound";
			break;
	}

	ImGui::Text("Type: %s", type.c_str());
	ImGui::Text("ID: %li", (uint64_t)asset.ID);
}

void AssetEditorPanel::Draw() {
	ImGui::Begin("Asset Editor", &m_Open);
	{
		if(m_CurrentAsset.Type != AssetType::None)
			EditAsset(m_CurrentAsset);
	}
	ImGui::End();
}

}