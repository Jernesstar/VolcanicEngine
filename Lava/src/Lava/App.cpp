#include "App.h"

#include <Magma/UI/UIRenderer.h>

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
	UIBrowser::OnUpdate(ts);
}

}