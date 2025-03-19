#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

class AssetEditorPanel : public Panel {
public:
	AssetEditorPanel();
	~AssetEditorPanel() = default;

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

	void Select(Asset asset) {
		m_CurrentAsset = asset;
	}

private:
	Asset m_CurrentAsset;
	std::string m_Path;
};

}