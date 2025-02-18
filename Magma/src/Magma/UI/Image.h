#pragma once

#include "UIElement.h"

#include <VolcaniCore/Graphics/Framebuffer.h>

namespace Magma::UI {

class Image : public UIElement {
public:
	Asset ImageAsset;

public:
	Image(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Image, id, root) { }

	void Draw() override;
};

}