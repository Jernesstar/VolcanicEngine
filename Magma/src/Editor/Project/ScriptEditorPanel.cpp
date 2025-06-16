#include "ScriptEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/UI/UIRenderer.h>

namespace fs = std::filesystem;

using namespace VolcaniCore;
using namespace Magma::UI;

namespace Magma {

ScriptEditorPanel::ScriptEditorPanel()
	: Panel("ScriptEditor")
{
	auto lang = TextEditor::LanguageDefinition::AngelScript();

	Map<std::string, std::string> identifiers =
	{
		{ "array", "class array" },
		{ "Vec3", "struct Vec3" },
		{ "Vec4", "struct Vec4" },
		{ "GridSet", "struct GridSet" },
		{ "Asset", "struct Asset" },
		{ "Entity", "struct Entity" },
		{ "CameraComponent", "class Components::CameraComponent" },
		{ "TransformComponent", "class Components::TransformComponent" },
		{ "EditorField", "EditorField: This field will be editable in the editor" },
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

void ScriptEditorPanel::EditFile(uint32_t i) {
	m_CurrentFile = i + 1;
	const auto& file = m_Files[i];
	m_Editor.SetText(FileUtils::ReadFile(file.Path));
	m_Editor.SetBreakpoints(file.Breakpoints);
}

void ScriptEditorPanel::OpenFile(const std::string& path) {
	auto [found, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });

	if(found && m_CurrentFile == i + 1)
		return;

	if(!found) {
		m_Files.Add({ path });
		EditFile(m_Files.Count() - 1);
	}
	else
		EditFile(i);
}

void ScriptEditorPanel::CloseFile(const std::string& path) {
	auto [found, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });
	if(!found)
		return;

	m_Files.Pop(i);
	if(!m_Files) {
		m_CurrentFile = 0;
		m_Editor.SetText("");
	}
	else
		EditFile(i == m_Files.Count() ? i - 1 : i);
}

ScriptFile& ScriptEditorPanel::GetFile(const std::string& path) {
	auto [_, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });
	// if(!found)
		// PANIC!!
	return m_Files[i];
}

ScriptFile& ScriptEditorPanel::Get() {
	return m_Files[m_CurrentFile - 1];
}

void ScriptEditorPanel::Update(TimeStep ts) {

}

void ScriptEditorPanel::Draw() {
	auto windowFlags = ImGuiWindowFlags_MenuBar
					 | ImGuiWindowFlags_NoNav;
	ImGui::Begin("Script Editor", &Open, windowFlags);

	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Open")) {
			
			}

			if(m_CurrentFile && ImGui::MenuItem("Save")) {
				std::string textToSave = m_Editor.GetText();
				FileUtils::WriteToFile(Get().Path, textToSave);
			}

			ImGui::EndMenu();
		}
		if(m_CurrentFile && ImGui::BeginMenu("Edit"))
		{
			bool ro = m_Editor.IsReadOnly();
			if(ImGui::MenuItem("Read-only mode", nullptr, &ro))
				m_Editor.SetReadOnly(ro);
			ImGui::Separator();

			if(ImGui::MenuItem("Undo", "ALT-Backspace", nullptr,
				!ro && m_Editor.CanUndo())) m_Editor.Undo();
			if(ImGui::MenuItem("Redo", "Ctrl-Y", nullptr,
				!ro && m_Editor.CanRedo())) m_Editor.Redo();

			ImGui::Separator();

			if(ImGui::MenuItem("Delete", "Del", nullptr,
				!ro && m_Editor.HasSelection())) m_Editor.Delete();
			if(ImGui::MenuItem("Copy", "Ctrl-C", nullptr,
				m_Editor.HasSelection())) m_Editor.Copy();
			if(ImGui::MenuItem("Cut", "Ctrl-X", nullptr,
				!ro && m_Editor.HasSelection())) m_Editor.Cut();
			if(ImGui::MenuItem("Paste", "Ctrl-V", nullptr,
				!ro && ImGui::GetClipboardText() != nullptr)) m_Editor.Paste();

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

	auto tabBarFlags = ImGuiTabBarFlags_Reorderable
					 | ImGuiTabBarFlags_TabListPopupButton;
	ImGui::BeginTabBar("ScriptFiles", tabBarFlags);
	{
		auto leadingFlags = ImGuiTabItemFlags_Trailing
							| ImGuiTabItemFlags_NoReorder;
		if(ImGui::TabItemButton("+", leadingFlags)) {

		}

		uint32_t fileToClose = 0;
		uint32_t i = 0;
		for(auto file : m_Files) {
			i++;
			auto name = fs::path(file.Path).filename().string();
			TabState state = UIRenderer::DrawTab(name);
			if(state.Closed)
				fileToClose = i;
			else if(state.Clicked)
				EditFile(i - 1);
		}

		if(fileToClose)
			CloseFile(m_Files[fileToClose - 1].Path);
	}
	ImGui::EndTabBar();

	if(!m_CurrentFile) {
		ImGui::End();
		return;
	}

	auto& file = Get();
	m_Editor.SetBreakpoints(file.Breakpoints);

	ImGui::Text("%d lines", m_Editor.GetTotalLines());
	
	m_Editor.Render("ScriptEditor");
	auto pos = m_Editor.GetCursorPosition();

	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) {
		std::string textToSave = m_Editor.GetText();
		FileUtils::WriteToFile(file.Path, textToSave);
	}
	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Backspace)) {
		
	}
	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_K)) {
		TextEditor::Coordinates start = { pos.mLine, 0 };
		TextEditor::Coordinates end = { pos.mLine, 0 };
		m_Editor.SetSelection(start, end, TextEditor::SelectionMode::Line);
		m_Editor.Delete();
		m_Editor.Delete();
		m_Editor.MoveLeft();
	}
	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_K)) {
		// Delete to the left
	}
	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_K)) {
		// Delete to the right
	}
	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_B)) {
		if(file.Breakpoints.contains(pos.mLine + 1))
			file.Breakpoints.erase(pos.mLine + 1);
		else
			file.Breakpoints.insert(pos.mLine + 1);
	}

	ImGui::End();
}

}