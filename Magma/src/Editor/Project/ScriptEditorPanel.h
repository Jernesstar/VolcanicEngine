#pragma once

#include <ImGuiColorTextEdit/TextEditor.h>

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

class ScriptEditorPanel : public Panel {
public:
	ScriptEditorPanel();
	~ScriptEditorPanel() = default;

	void EditFile(const Asset& asset);

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

private:
	Asset m_CurrentAsset;
	TextEditor m_Editor;
};

}