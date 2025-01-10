#pragma once

#include "Panel.h"

using namespace VolcaniCore;

namespace Magma {

class ContentBrowserPanel : public Panel  {
public:
	ContentBrowserPanel(const std::string& path);
	~ContentBrowserPanel() = default;

	void Update(TimeStep ts);
	void Render();

private:
	std::string m_Path;
};

}