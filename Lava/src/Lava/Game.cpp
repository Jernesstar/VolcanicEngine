#include "Game.h"

#include <Magma/UI/UI.h>

#include "UIBrowser.h"

using namespace Magma::UI;

namespace Lava {

void Game::OnLoad() {
	UIRenderer::Init();
}

void Game::OnClose() {
	UIRenderer::Close();
}

void Game::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();
	UIBrowser::OnUpdate(ts);
	UIRenderer::EndFrame();
}

}