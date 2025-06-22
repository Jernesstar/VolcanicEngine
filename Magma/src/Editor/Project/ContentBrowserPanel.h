#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/UI/Image.h>
#include <Magma/Core/AssetManager.h>

#include "Editor/Panel.h"

namespace fs = std::filesystem;

namespace Magma {

class ContentBrowserPanel : public Panel  {
public:
	ContentBrowserPanel();
	~ContentBrowserPanel();

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

	void Select(AssetType type, uint32_t id = 0);
	bool IsSelecting();
	void CancelSelect();
	bool HasSelection(uint32_t id = 0);
	Asset GetSelected();

private:
	void RenderAssetTable();
};

}