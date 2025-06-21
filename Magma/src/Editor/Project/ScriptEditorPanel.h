#pragma once

#include <ImGuiColorTextEdit/TextEditor.h>

#include <Magma/UI/Button.h>

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

struct ScriptFile {
	const std::string Path;
	TextEditor::Breakpoints Breakpoints;
	TextEditor::ErrorMarkers Errors;
	bool Open = true;
};

class ScriptEditorPanel : public Panel {
public:
	ScriptEditorPanel();
	~ScriptEditorPanel();

	void OpenFile(const std::string& path, bool open = true);
	void CloseFile(const std::string& path);
	ScriptFile* GetFile(const std::string& path);
	ScriptFile* GetFile();
	void SetDebugLine(uint32_t line);

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

private:
	VolcaniCore::List<ScriptFile> m_Files;
	uint32_t m_OpenCount = 0;
	uint32_t m_CurrentFile = 0;
	UI::Button m_Debug, m_StepOver, m_StepInto, m_StepOut, m_Continue;
	TextEditor m_Editor;

private:
	void EditFile(uint32_t i);
};

}