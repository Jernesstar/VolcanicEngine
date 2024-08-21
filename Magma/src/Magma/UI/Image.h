#pragma once

#include "UIElement.h"

namespace Magma::UI {

class Image : public UIElement {
public:
	Image(Ref<Texture> image = nullptr);
	Image(const std::string& imagePath);

	void SetImage(Ref<Texture> texture);
	void SetImage(const std::string& imagePath);

	Ref<Texture> GetImage() { return m_Image; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	Ref<Texture> m_Image;
};

}