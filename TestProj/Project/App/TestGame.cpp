#include "TestGame.h"

namespace TestProj {

void TestGame::OnLoad() {
	VOLCANICORE_LOG_INFO("OnLoad");
}

void TestGame::OnClose() {
	VOLCANICORE_LOG_INFO("OnClose");
}

void TestGame::OnUpdate(TimeStep ts) {
	// VOLCANICORE_LOG_INFO("OnUpdate");
}

}