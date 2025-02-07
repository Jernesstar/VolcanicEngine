#include "Image.h"

#include "UIRenderer.h"

using namespace VolcaniCore;

namespace Magma::UI {

Image::Image(Ref<Texture> image)
	: UIElement(UIElementType::Image)
{
	SetImage(image);
}

Image::Image(const std::string& imagePath)
	: UIElement(UIElementType::Image)
{
	SetImage(imagePath);
}

void Image::SetImage(Ref<Texture> image) {
	if(!image)
		return;

	Content = image;
	Width = Content->GetWidth();
	Height = Content->GetHeight();
}

void Image::SetImage(const std::string& imagePath) {
	if(imagePath == "")
		return;

	SetImage(Texture::Create(imagePath));
}

void Image::Draw() {
	m_State = UIRenderer::DrawImage(*this);
}

}