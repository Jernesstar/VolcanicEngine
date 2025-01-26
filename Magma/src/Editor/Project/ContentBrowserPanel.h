#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/Panel.h"

#include <Magma/UI/Image.h>

namespace fs = std::filesystem;

using namespace VolcaniCore;

namespace Magma {

class ContentBrowserPanel : public Panel  {
public:
	ContentBrowserPanel(const std::string& path);
	~ContentBrowserPanel() = default;

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	fs::path m_Path;
	fs::path m_CurrentPath;
	fs::path m_AssetPath;
	Ref<UI::Image> m_FileIcon;
	Ref<UI::Image> m_FolderIcon;
};

}