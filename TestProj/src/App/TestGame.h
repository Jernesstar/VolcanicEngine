#pragma once

#include <VolcaniCore/Core/Log.h>

#include <Lava/App.h>

namespace TestProj {

class TestGame : public Lava::App {
public:
	TestGame() = default;

	void OnLoad() override;
	void OnClose() override;
	void OnUpdate(TimeStep ts) override;
};

}