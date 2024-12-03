#include "Image.h"

#include <OpenGL/Framebuffer.h>
#include <OpenGL/Texture2D.h>

#include "UIPage.h"

using namespace VolcaniCore;

namespace Magma::UI {

Image::Image(Ref<Texture> image)
	: UIElement(UIElement::Type::Image)
{
	SetImage(image);
}

Image::Image(const std::string& imagePath)
	: UIElement(UIElement::Type::Image)
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

void Image::SetImage(Ref<Framebuffer> framebuffer, AttachmentTarget target) {
	// TODO(Change): Move to texture
	auto& attachment = framebuffer->As<OpenGL::Framebuffer>()->Get(target);
	Content->As<OpenGL::Texture2D>()->SetID(attachment.GetRendererID());
	Width = attachment.GetWidth();
	Height = attachment.GetHeight();
}

void Image::Draw() {
	m_State = UIRenderer::DrawImage(*this);
	if(m_Root)
		m_Root->OnEvent(m_State, m_ID);
}

}