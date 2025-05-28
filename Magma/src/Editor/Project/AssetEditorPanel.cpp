#include "AssetEditorPanel.h"

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include "Editor/EditorApp.h"

#include "Project/ScriptEditorPanel.h"

namespace fs = std::filesystem;

using namespace VolcaniCore;

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
		case AssetType::Script:
			type = "Script";
			break;
		case AssetType::Audio:
			type = "Audio";
			break;
	}

	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto* assets = AssetManager::Get()->As<EditorAssetManager>();

	ImGui::Text("Type: %s", type.c_str());
	ImGui::Text("ID: %llu", (uint64_t)asset.ID);
	auto path = assets->GetPath(asset.ID);
	if(path != "") {
		auto rootPath = fs::path(editor.GetProject().Path) / "Asset";
		auto display = fs::relative(path, rootPath).generic_string();
		ImGui::Text("Path: %s", display.c_str());
	}

	std::string name = assets->GetAssetName(asset);
	ImGui::Text("Name: "); ImGui::SameLine();
	bool active = ImGui::InputTextWithHint("##AssetName", "Unnamed asset", &name);

	if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
		assets->RemoveName(asset);
		assets->NameAsset(asset, name);
	}

	if(asset.Type == AssetType::Script) {
		if(ImGui::Button("Edit Script")) {
			auto tab = editor.GetProjectTab();
			auto panel = tab->GetPanel("ScriptEditor")->As<ScriptEditorPanel>();
			panel->Open = true;
			panel->EditFile(asset);
		}
	}
}

void AssetEditorPanel::Draw() {
	ImGui::Begin("Asset Editor");
	{
		if(m_CurrentAsset.Type != AssetType::None)
			EditAsset(m_CurrentAsset);
	}
	ImGui::End();
}

}