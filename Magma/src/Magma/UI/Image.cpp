#include "Image.h"

#include <OpenGL/Texture2D.h>

using namespace VolcaniCore;

namespace Magma::UI {

Image::Image(Ref<Texture> image)
	: UIElement(UIElement::Type::Image, image->GetWidth(), image->GetHeight()),
		m_Image(image) { }

Image::Image(const std::string& imagePath)
	: UIElement(UIElement::Type::Image)
{
	Set(imagePath);
}

void Image::Set(Ref<Texture> image) {
	m_Image = image;
	m_Width = m_Image->GetWidth();
	m_Height = m_Image->GetHeight();
}

void Image::Set(const std::string& path) {
	m_Image = Texture::Create(imagePath);
	m_Width = m_Image->GetWidth();
	m_Height = m_Image->GetHeight();
}

void Image::Draw() {
	auto image = m_Image->As<OpenGL::Texture2D>();
	image->Bind();
	ImGui::Image((void*)(uint64_t)(uint32_t)image->GetID(),
				 ImVec2(m_Width, m_Width));
}

bool Image::OnAttach() {
	if(m_Parent->GetType() == UIElement::Type::Button)
		return false; // The button captures the image

	return true;
}

bool Image::OnAddElement(Ref<UIElement> element) {
	return false;
}

}