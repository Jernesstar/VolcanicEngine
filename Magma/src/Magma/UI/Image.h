#pragma once

#include "UIElement.h"

#include <VolcaniCore/Object/Framebuffer.h>

namespace Magma::UI {

class Image : public UIElement {
public:
	struct Specification {
		std::string Path = "";

		float x = 0;
		float y = 0;
		uint32_t Width = 0;
		uint32_t Height = 0;
	};

public:
	Ref<Texture> Content;

public:
	Image(Ref<Texture> image = nullptr);
	Image(const std::string& imagePath);
	Image(const std::string& id, UIPage* root)
	: UIElement(UIElement::Type::Image, id, root) { }

	void SetImage(Ref<Texture> texture);
	void SetImage(const std::string& imagePath);
	// TODO(Change): Move this to Texture
	void SetImage(Ref<Framebuffer> framebuffer, AttachmentTarget target);

private:
	void Draw() override;
};

}