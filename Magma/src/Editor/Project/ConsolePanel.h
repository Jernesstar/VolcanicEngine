#pragma once

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

enum class ConsoleLevel { Info, Warn, Error };

class ConsolePanel : public Panel {
public:
	ConsolePanel();
	~ConsolePanel() = default;

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

	void Log(ConsoleLevel level, const std::string& message);

private:
	VolcaniCore::List<std::pair<ConsoleLevel, std::string>> m_Log;
};

}