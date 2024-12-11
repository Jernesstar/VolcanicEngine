#pragma once

#include "UIElement.h"

#include <VolcaniCore/Graphics/Framebuffer.h>

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
	static Ref<UI::Image> Create(const UI::Image::Specification& specs);

public:
	Ref<Texture> Content;

public:
	Image(Ref<Texture> image = nullptr);
	Image(const std::string& imagePath);

	void SetImage(Ref<Texture> texture);
	void SetImage(const std::string& imagePath);
	// TODO(Change): Move this to Texture
	void SetImage(Ref<Framebuffer> framebuffer, AttachmentTarget target);

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;
};

}