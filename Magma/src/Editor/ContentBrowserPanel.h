#pragma once

#include "Panel.h"

using namespace VolcaniCore;

namespace Magma {

class ContentBrowserPanel : public Panel  {
public:
	ContentBrowserPanel();
	~ContentBrowserPanel() = default;

	void Update(TimeStep ts);
	void Render();

private:
	
};

}