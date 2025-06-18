#include "ScriptEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>
#include <Magma/UI/UIRenderer.h>

#include "Editor/Editor.h"
#include "Editor/AssetImporter.h"
#include "Editor/ScriptManager.h"

namespace fs = std::filesystem;

using namespace VolcaniCore;
using namespace Magma::UI;

namespace Magma {

ScriptEditorPanel::ScriptEditorPanel()
	: Panel("ScriptEditor")
{
	Application::PushDir();
	m_Debug.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/Debug.png"));
	m_StepOver.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StepOver.png"));
	m_StepInto.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StepInto.png"));
	m_StepOut.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StepOut.png"));
	m_Continue.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/Continue.png"));

	m_Debug.UsePosition = false;
	m_Debug.SetSize(17, 17);
	m_StepOver.UsePosition = false;
	m_StepOver.SetSize(17, 17);
	m_StepInto.UsePosition = false;
	m_StepInto.SetSize(17, 17);
	m_StepOut.UsePosition = false;
	m_StepOut.SetSize(17, 17);
	m_Continue.UsePosition = false;
	m_Continue.SetSize(17, 17);

	Application::PopDir();

	auto path = Editor::GetProject().Path;
	path = (fs::path(path) / "Editor" / "Scripts.yaml").string();
	if(!FileUtils::FileExists(path))
		return;

	YAML::Node rootNode;
	try {
		rootNode = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								path.c_str(), e.what());
	}

	for(auto node : rootNode["Scripts"]) {
		auto file = node["Script"];
		auto path = file["Path"].as<std::string>();
		auto list = file["Breakpoints"].as<std::vector<int>>();
		TextEditor::Breakpoints bps(list.begin(), list.end());
		auto open = file["Open"].as<bool>();
		m_Files.Add({ .Path = path, .Breakpoints = bps, .Open = open });
		m_OpenCount += open;
	}
	if(m_OpenCount) {
		Open = true;
		EditFile(0);
	}

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
		{ "EditorField", "This field will be editable in the editor" },
	};
	for(auto [decl, def] : identifiers) {
		TextEditor::Identifier id;
		id.mDeclaration = def;
		lang.mIdentifiers.insert({ decl, id });
	}

	m_Editor.SetLanguageDefinition(lang);
	auto palette = TextEditor::GetDarkPalette();
	m_Editor.SetPalette(palette);

	m_Debugging = false;
}

ScriptEditorPanel::~ScriptEditorPanel() {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File
	serializer.WriteKey("Scripts").BeginSequence();
	for(auto& file : m_Files) {
		serializer.BeginMapping()
			.WriteKey("Script").BeginMapping();
		serializer.WriteKey("Path").Write(file.Path);

		serializer.WriteKey("Breakpoints")
			.SetOptions(Serializer::Options::ArrayOneLine)
			.BeginSequence();
		for(int bp : file.Breakpoints)
			serializer.Write(bp);
		serializer
			.EndSequence();

		serializer.WriteKey("Open").Write(file.Open);

		serializer.EndMapping()
			.EndMapping();
	}
	serializer.EndSequence();
	serializer.EndMapping();

	auto path = Editor::GetProject().Path;
	path = (fs::path(path) / "Editor" / "Scripts.yaml").string();
	serializer.Finalize(path);

	if(m_Debugging)
		Editor::GetProjectTab()->OnStop();
}

void ScriptEditorPanel::EditFile(uint32_t i) {
	m_CurrentFile = i;
	auto& file = m_Files[i];
	file.Open = true;
	m_Editor.SetText(FileUtils::ReadFile(file.Path));
}

void ScriptEditorPanel::OpenFile(const std::string& path, bool open) {
	auto [found, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });

	if(found && m_OpenCount && m_CurrentFile == i)
		return;

	if(open)
		m_OpenCount++;

	if(!found) {
		m_Files.Add({ .Path = path, .Open = open });
		if(open)
			EditFile(m_Files.Count() - 1);
	}
	else if(open)
		EditFile(i);
}

void ScriptEditorPanel::CloseFile(const std::string& path) {
	auto [found, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });
	if(!found)
		return;

	GetFile()->Open = false;
	if(m_OpenCount)
		m_OpenCount--;

	if(!m_OpenCount) {
		m_CurrentFile = 0;
		m_Editor.SetText("");
		return;
	}

	auto [_,idx] =
		m_Files.Find([](auto& file) { return file.Open; });
	EditFile(idx);
}

ScriptFile* ScriptEditorPanel::GetFile(const std::string& path) {
	auto [found, i] =
		m_Files.Find([path](auto& file) { return file.Path == path; });
	if(!found)
		return nullptr;
	return m_Files.At(i);
}

ScriptFile* ScriptEditorPanel::GetFile() {
	return m_Files.At(m_CurrentFile);
}

void ScriptEditorPanel::EndDebug() {
	m_Debugging = false;
	ScriptManager::EndDebug();
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

			if(m_OpenCount && ImGui::MenuItem("Save")) {
				std::string textToSave = m_Editor.GetText();
				FileUtils::WriteToFile(GetFile()->Path, textToSave);
			}

			ImGui::EndMenu();
		}
		if(m_OpenCount && ImGui::BeginMenu("Edit"))
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
			if(!file.Open)
				continue;

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

	if(!m_OpenCount) {
		// Open new file prompt

		ImGui::End();
		return;
	}

	auto* file = GetFile();
	m_Editor.SetBreakpoints(file->Breakpoints);
	m_Editor.SetErrorMarkers(file->Errors);

	ImGui::Text("%d lines", m_Editor.GetTotalLines());
	ImGui::SameLine(0.0f, m_Debugging ? 75.0f : 100.0f);
	m_Debug.Render();

	if(m_Debug.GetState().Clicked) {
		if(!m_Debugging) {
			ScriptManager::StartDebug();
			Editor::GetProjectTab()->OnPlay();
			m_Debugging = true;
		}
		else {
			Editor::GetProjectTab()->OnStop();
			m_Debugging = false;
		}
	}

	if(m_Debugging) {
		ImGui::SameLine();
		m_Continue.Render(); ImGui::SameLine();
		m_StepOver.Render(); ImGui::SameLine();
		m_StepInto.Render(); ImGui::SameLine();
		m_StepOut.Render();

		if(m_Continue.GetState().Clicked)
			ScriptManager::Continue();
		if(m_StepOver.GetState().Clicked)
			ScriptManager::StepOver();
		if(m_StepInto.GetState().Clicked)
			ScriptManager::StepInto();
		if(m_StepOut.GetState().Clicked)
			ScriptManager::StepOut();
	}

	m_Editor.Render("ScriptEditor");

	auto pos = m_Editor.GetCursorPosition();

	if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) {
		std::string textToSave = m_Editor.GetText();
		FileUtils::WriteToFile(file->Path, textToSave);
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
		if(file->Breakpoints.contains(pos.mLine + 1))
			file->Breakpoints.erase(pos.mLine + 1);
		else
			file->Breakpoints.insert(pos.mLine + 1);
	}

	ImGui::End();
}

}