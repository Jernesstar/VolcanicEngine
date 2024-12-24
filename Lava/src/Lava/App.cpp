#include "App.h"

#include <Magma/UI/UI.h>

#include "UIBrowser.h"

using namespace Magma::UI;

namespace Lava {

void App::OnLoad() {
	UIRenderer::Init();
}

void App::OnClose() {
	UIRenderer::Close();
}

void App::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();
	UIBrowser::OnUpdate(ts);
	UIRenderer::EndFrame();
}

}