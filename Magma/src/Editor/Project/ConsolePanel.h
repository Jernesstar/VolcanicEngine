#pragma once

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

class ConsolePanel : public Panel {
public:
	ConsolePanel();
	~ConsolePanel() = default;

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

private:

};

}