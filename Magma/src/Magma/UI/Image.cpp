#include "Image.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

Image::Image(Ref<Texture> image)
	: UIElement(UIType::Image, image->Width, image->Height), m_Image(image) { }
Image::Image(const std::string& imagePath)
	: UIElement(UIType::Image)
{
	m_Image = CreateRef<Texture>(imagePath);
	m_Width = m_Image->GetWidth();
	m_Height = m_Image->GetHeight();
}

void Image::Draw() {
	ImGui::Image((void*)m_Texture->As<OpenGL::Texture2D>()->GetID(),
					ImVec2{ m_Width, m_Width });
}

bool Image::OnAttach() {
	if(m_Parent->Type == UIType::Button)
		return false; // The button captures the image

	return true;
}

bool Image::OnAddElement(Ref<UIElement> element) {
	return false;
}

}