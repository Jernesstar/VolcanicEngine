#include "ScriptEditorPanel.h"

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

using namespace VolcaniCore;

namespace Magma {

ScriptEditorPanel::ScriptEditorPanel()
	: Panel("ScriptEditor")
{
	auto lang = TextEditor::LanguageDefinition::AngelScript();

	Map<std::string, std::string> identifiers =
	{
		{ "array", "class array" },
		{ "Vec3", "class Vec3" },
		{ "Vec4", "class Vec4" },
		{ "Asset", "class Asset" },
		{ "Entity", "class Entity" },
		{ "TransformComponent", "class Components::TransformComponent" },
	};
	for(auto [decl, def] : identifiers) {
		TextEditor::Identifier id;
		id.mDeclaration = def;
		lang.mIdentifiers.insert({ decl, id });
	}

	m_Editor.SetLanguageDefinition(lang);

	auto palette = TextEditor::GetDarkPalette();

	m_Editor.SetPalette(palette);
}

void ScriptEditorPanel::EditFile(const Asset& asset) {
	m_CurrentAsset = asset;
	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();
	std::string path = assetManager->GetPath(asset.ID);
	std::string text = FileUtils::ReadFile(path);

	m_Editor.SetText(text);
}

void ScriptEditorPanel::Update(TimeStep ts) {

}

void ScriptEditorPanel::Draw() {
	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();
	auto path = assetManager->GetPath(m_CurrentAsset.ID);

	ImGui::Begin("Script Editor", &Open,
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Save")) {
				std::string textToSave = m_Editor.GetText();
				FileUtils::WriteToFile(path, textToSave);
			}
			if(ImGui::MenuItem("Quit", "Alt-F4"))
				Open = false;
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit"))
		{
			bool ro = m_Editor.IsReadOnly();
			if(ImGui::MenuItem("Read-only mode", nullptr, &ro))
				m_Editor.SetReadOnly(ro);
			ImGui::Separator();

			if(ImGui::MenuItem("Undo", "ALT-Backspace", nullptr,
				!ro && m_Editor.CanUndo()))
				m_Editor.Undo();
			if(ImGui::MenuItem("Redo", "Ctrl-Y", nullptr,
				!ro && m_Editor.CanRedo()))
				m_Editor.Redo();

			ImGui::Separator();

			if(ImGui::MenuItem("Copy", "Ctrl-C", nullptr,
				m_Editor.HasSelection()))
				m_Editor.Copy();
			if(ImGui::MenuItem("Cut", "Ctrl-X", nullptr,
				!ro && m_Editor.HasSelection()))
				m_Editor.Cut();
			if(ImGui::MenuItem("Delete", "Del", nullptr,
				!ro && m_Editor.HasSelection()))
				m_Editor.Delete();
			if(ImGui::MenuItem("Paste", "Ctrl-V", nullptr,
				!ro && ImGui::GetClipboardText() != nullptr))
				m_Editor.Paste();

			ImGui::Separator();

			if(ImGui::MenuItem("Select all", nullptr, nullptr))
				m_Editor.SetSelection(TextEditor::Coordinates(),
					TextEditor::Coordinates(m_Editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem("Dark palette"))
				m_Editor.SetPalette(TextEditor::GetDarkPalette());
			if(ImGui::MenuItem("Light palette"))
				m_Editor.SetPalette(TextEditor::GetLightPalette());
			if(ImGui::MenuItem("Retro blue palette"))
				m_Editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	auto cpos = m_Editor.GetCursorPosition();
	ImGui::Text("%3d/%-3d %3d lines | %s | %s | %s | %s",
		cpos.mLine + 1, cpos.mColumn + 1, m_Editor.GetTotalLines(),
		m_Editor.IsOverwrite() ? "Ovr" : "Ins",
		m_Editor.CanUndo() ? "*" : " ",
		m_Editor.GetLanguageDefinition().mName.c_str(), path.c_str());

	m_Editor.Render("ScriptEditor");

	ImGui::End();
}

}