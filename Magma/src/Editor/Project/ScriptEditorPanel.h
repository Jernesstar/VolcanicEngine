#pragma once

#include <angelscript/add_on/debugger/debugger.h>

#include <ImGuiColorTextEdit/TextEditor.h>

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

struct ScriptFile {
	const std::string Path;
	TextEditor::Breakpoints Breakpoints;
};

class ScriptEditorPanel : public Panel {
public:
	ScriptEditorPanel();
	~ScriptEditorPanel() = default;

	void OpenFile(const std::string& path);
	void CloseFile(const std::string& path);

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

private:
	VolcaniCore::List<ScriptFile> m_Files;
	bool m_OpenFile;
	uint32_t m_CurrentFile;
	TextEditor m_Editor;
	CDebugger m_Debugger;

private:
	void EditFile(uint32_t i);
};

}