#include "AssetEditorPanel.h"

#include <SPIRV-Cross/spirv_glsl.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/EditorApp.h"
#include "Editor/AssetImporter.h"
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

	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();

	ImGui::Text("Type: %s", AssetTypeToString(asset.Type).c_str());
	ImGui::Text("ID: %llu", (uint64_t)asset.ID);
	auto path = assetManager->GetPath(asset.ID);
	if(path != "") {
		auto rootPath = fs::path(editor.GetProject().Path) / "Asset";
		auto display = fs::relative(path, rootPath).generic_string();
		ImGui::Text("Path: %s", display.c_str());
	}

	std::string name = assetManager->GetAssetName(asset);
	ImGui::Text("Name: "); ImGui::SameLine();
	bool active = ImGui::InputTextWithHint("##Name", "Unnamed asset", &name);

	if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
		assetManager->RemoveName(asset);
		assetManager->NameAsset(asset, name);
	}

	if(asset.Type == AssetType::Script) {
		if(ImGui::Button("Edit Script")) {
			auto tab = editor.GetProjectTab();
			auto panel = tab->GetPanel("ScriptEditor")->As<ScriptEditorPanel>();
			panel->Open = true;
			panel->OpenFile(path);
		}
	}
	else if(asset.Type == AssetType::Shader) {
		if(ImGui::Button("Create New Material")) {
			Asset newMaterial = assetManager->Add(AssetType::Material);
			
		}
	}
	else if(asset.Type == AssetType::Material) {
		auto mat = assetManager->Get<Material>(asset);

		ImGui::SeparatorText("Int Uniforms");
		for(auto& [name, data] : mat->IntUniforms) {
			ImGui::Text(name.c_str()); ImGui::SameLine();
			ImGui::DragScalar(std::string("##S32##" + name).c_str(),
								ImGuiDataType_S32, &data);
		}
		if(ImGui::Button("+")) {
			
		}

		// Asset shader = assetManager->GetRefs(asset)[0];
		// auto path = assetManager->GetPath(shader.ID);

		// Buffer<uint32_t> data = AssetImporter::GetShaderData(path);

		// spirv_cross::Compiler compiler(data.Get(), data.GetCount());
		// spirv_cross::ShaderResources resources =
		// 	compiler.get_shader_resources();
		// for(const auto& resource : resources.gl_plain_uniforms) {
		// 	ImGui::Text(resource.name.c_str());
			
		// }
		// for(const auto& resource : resources.separate_samplers) {
		// 	ImGui::Text(resource.name.c_str());
			
		// }
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