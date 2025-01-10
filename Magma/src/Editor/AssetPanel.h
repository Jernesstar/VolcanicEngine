#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include "Panel.h"

namespace Magma {

class AssetPanel : public Panel {
public:
	AssetPanel(const std::string& path);
	~AssetPanel() = default;

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	std::string m_Path;
};

}