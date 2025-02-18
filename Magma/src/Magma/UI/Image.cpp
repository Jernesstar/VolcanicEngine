#include "Image.h"

#include "UIRenderer.h"

using namespace VolcaniCore;

namespace Magma::UI {

void Image::Draw() {
	m_State = UIRenderer::DrawImage(*this);
}

}