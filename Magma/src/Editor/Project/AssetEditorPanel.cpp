#include "AssetEditorPanel.h"

#include <SPIRV-Cross/spirv_glsl.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Math.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/EditorApp.h"
#include "Editor/AssetImporter.h"
#include "Project/ContentBrowserPanel.h"
#include "Project/ScriptEditorPanel.h"

namespace fs = std::filesystem;

using namespace VolcaniCore;

namespace Magma {

AssetEditorPanel::AssetEditorPanel()
	: Panel("AssetEditor")
{
}

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

	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();

	ImGui::Text("Type: %s", AssetTypeToString(asset.Type).c_str());
	ImGui::Text("ID: %llu", (uint64_t)asset.ID);
	auto path = assetManager->GetPath(asset.ID);
	if(path != "") {
		auto rootPath = fs::path(Editor::GetProject().Path) / "Asset";
		auto display = fs::relative(path, rootPath).generic_string();
		ImGui::Text("Path: %s", display.c_str());
	}

	std::string name = assetManager->GetAssetName(asset);
	ImGui::Text("Name: "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150);
	bool active = ImGui::InputTextWithHint("##Name", "Unnamed asset", &name);

	if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
		assetManager->RemoveName(asset);
		assetManager->NameAsset(asset, name);
	}

	if(asset.Type == AssetType::Script) {
		if(ImGui::Button("Edit Script")) {
			auto tab = Editor::GetProjectTab();
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

	if(asset.Type != AssetType::Material)
		return;

	auto mat = assetManager->Get<Material>(asset);

	ImGui::SeparatorText("Int Uniforms");
	for(auto& [name, data] : mat->IntUniforms) {
		ImGui::PushID((name + "IntUniforms").c_str());

		if(ImGui::Button("Delete"))
			mat->IntUniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		ImGui::SetNextItemWidth(150);
		bool active = ImGui::InputText("##Name", &str);
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->IntUniforms.erase(name);
			mat->IntUniforms[str] = data;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		ImGui::DragScalar("##Data", ImGuiDataType_S32, &data);

		ImGui::PopID();
	}
	if(ImGui::Button("+##IntUniforms")) {
		std::string name = "IntUniforms"
			+ std::to_string(mat->IntUniforms.size());
		mat->IntUniforms[name] = 0;
	}

	ImGui::SeparatorText("Float Uniforms");
	for(auto& [name, data] : mat->FloatUniforms) {
		ImGui::PushID((name + "FloatUniforms").c_str());

		if(ImGui::Button("Delete"))
			mat->FloatUniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		ImGui::SetNextItemWidth(150);
		bool active = ImGui::InputText("##Name", &str);
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->FloatUniforms.erase(name);
			mat->FloatUniforms[str] = data;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat("##Data", &data, 0.01f);

		ImGui::PopID();
	}
	if(ImGui::Button("+##FloatUniforms")) {
		std::string name = "FloatUniforms"
			+ std::to_string(mat->FloatUniforms.size());
		mat->FloatUniforms[name] = 0.0f;
	}

	ImGui::SeparatorText("Vec2 Uniforms");
	for(auto& [name, data] : mat->Vec2Uniforms) {
		ImGui::PushID((name + "Vec2Uniforms").c_str());

		if(ImGui::Button("Delete"))
			mat->Vec2Uniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		bool active = ImGui::InputText("##Name", &str);
		ImGui::SetNextItemWidth(150);
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->Vec2Uniforms.erase(name);
			mat->Vec2Uniforms[str] = data;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat2("##Data", &data.x, 0.01f);

		ImGui::PopID();
	}
	if(ImGui::Button("+##Vec2Uniforms")) {
		std::string name = "Vec2Uniforms"
			+ std::to_string(mat->Vec2Uniforms.size());
		mat->Vec2Uniforms[name] = Vec2(0.0f);
	}

	ImGui::SeparatorText("Vec3 Uniforms");
	for(auto& [name, data] : mat->Vec3Uniforms) {
		ImGui::PushID((name + "Vec3Uniforms").c_str());

		if(ImGui::Button("Delete"))
			mat->Vec3Uniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		ImGui::SetNextItemWidth(150);
		bool active = ImGui::InputText("##Data", &str);
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->Vec3Uniforms.erase(name);
			mat->Vec3Uniforms[str] = data;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat3("##Data", &data.x, 0.01f);

		ImGui::PopID();
	}
	if(ImGui::Button("+##Vec3Uniforms")) {
		std::string name = "Vec3Uniforms"
			+ std::to_string(mat->Vec3Uniforms.size());
		mat->Vec3Uniforms[name] = Vec3(0.0f);
	}

	ImGui::SeparatorText("Vec4 Uniforms");
	for(auto& [name, data] : mat->Vec4Uniforms) {
		ImGui::PushID((name + "Vec4Uniforms").c_str());
		
		if(ImGui::Button("Delete"))
			mat->Vec4Uniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		ImGui::SetNextItemWidth(150);
		bool active = ImGui::InputText("##Name", &str);
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->Vec4Uniforms.erase(name);
			mat->Vec4Uniforms[str] = data;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::DragFloat4("##Data", &data.x, 0.01f);

		ImGui::PopID();
	}
	if(ImGui::Button("+##Vec4Uniforms")) {
		std::string name = "Vec4Uniforms"
			+ std::to_string(mat->Vec4Uniforms.size());
		mat->Vec4Uniforms[name] = Vec4(0.0f);
	}

	ImGui::SeparatorText("Texture Uniforms");
	uint32_t i = 0;
	for(auto& [name, data] : mat->TextureUniforms) {
		ImGui::PushID((name + "TextureUniforms").c_str());

		if(ImGui::Button("Delete"))
			mat->TextureUniforms.erase(name);
		ImGui::SameLine();

		std::string str = name;
		ImGui::SetNextItemWidth(150);
		bool active = ImGui::InputText("##Name", &str);

		if(ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();

			if(data) {
				Asset asset = { data, AssetType::Texture };
				assetManager->Load(asset);
				Ref<Texture> texture = assetManager->Get<Texture>(asset);

				UI::Image image;
				image.x = 5.0f;
				image.Width = 120;
				image.Height = 120;
				image.Content = texture;
				// image.Content = s_Thumbnails[data];
				image.Render();

				auto name = assetManager->GetAssetName(asset);
				auto path = assetManager->GetPath(data);
				if(name != "")
					ImGui::Text(name.c_str());
				else if(path != "")
					ImGui::Text(fs::path(path).filename().string().c_str());
				else
					ImGui::Text("%llu", (uint64_t)data);
			}
			else {
				ImGui::Text("No Asset Selected");
				// TODO(Art): Little indent with barred circle or something
			}

			ImGui::EndTooltip();
		}
		if(active && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
			mat->TextureUniforms.erase(name);
			mat->TextureUniforms[str] = data;
		}

		auto panel =
			Editor::GetProjectTab()->
					GetPanel("ContentBrowser")->As<ContentBrowserPanel>();
		ImGui::SameLine();
		if(ImGui::Button("Select"))
			panel->Select(AssetType::Texture, i);
		if(panel->HasSelection(i))
			mat->TextureUniforms[name] = panel->GetSelected().ID;

		i++;
		ImGui::PopID();
	}
	if(ImGui::Button("+##TextureUniforms")) {
		std::string name = "TextureUniforms"
			+ std::to_string(mat->TextureUniforms.size());
		mat->TextureUniforms[name] = 0;
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

void AssetEditorPanel::Draw() {
	ImGui::Begin("Asset Editor");
	{
		if(m_CurrentAsset.Type != AssetType::None)
			EditAsset(m_CurrentAsset);
	}
	ImGui::End();
}

}