#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/Panel.h"

#include <Magma/UI/Image.h>

namespace fs = std::filesystem;

namespace Magma {

class ContentBrowserPanel : public Panel  {
public:
	ContentBrowserPanel(const std::string& path);
	~ContentBrowserPanel();

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

	void RenderAssetTable(bool hovering = false);

private:
	fs::path m_Path;
	Ref<UI::Image> m_FileIcon;
	Ref<UI::Image> m_FolderIcon;
};

}