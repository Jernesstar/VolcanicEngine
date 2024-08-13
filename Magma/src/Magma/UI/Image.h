#pragma once

#include "UI.h"

namespace Magma::UI {

class Image : public UIElement {
public:
	Image(Ref<Texture> image);
	Image(const std::string& imagePath);

	void Set(Ref<Texture> texture);
	void Set(const std::string& imagePath);

	Ref<Texture> GetImage() { return m_Image; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	Ref<Texture> m_Image;
};

}