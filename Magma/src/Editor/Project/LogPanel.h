#pragma once

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

class LogPanel : public Panel {
public:
	LogPanel();
	~LogPanel() = default;

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

private:
};

}