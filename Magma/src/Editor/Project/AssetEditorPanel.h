#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/Panel.h"

namespace Magma {

class AssetEditorPanel : public Panel {
public:
	AssetEditorPanel(const std::string& path);
	~AssetEditorPanel() = default;

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	std::string m_Path;
};

}