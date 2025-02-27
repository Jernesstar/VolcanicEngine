#pragma once

#include <VolcaniCore/Graphics/Framebuffer.h>

#include "Core/AssetManager.h"

#include "UIElement.h"

namespace Magma::UI {

class Image : public UIElement {
public:
	Ref<Texture> Content;
	uint64_t ImageID = 0;

public:
	Image()
		: UIElement(UIElementType::Image) { }
	Image(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Image, id, root) { }
	Image(Ref<Texture> image)
		: UIElement(UIElementType::Image), Content(image) { }

	void Draw() override;
};

}