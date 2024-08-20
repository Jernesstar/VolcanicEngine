#include "Image.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <OpenGL/Texture2D.h>

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
	Set(imagePath);
}

void Image::Set(Ref<Texture> image) {
	if(!image)
		return;
	m_Image = image;
	m_Width = m_Image->GetWidth();
	m_Height = m_Image->GetHeight();
}

void Image::Set(const std::string& imagePath) {
	if(imagePath == "")
		return;
	m_Image = Texture::Create(imagePath);
	m_Width = m_Image->GetWidth();
	m_Height = m_Image->GetHeight();
}

void Image::Draw() {
	auto image = m_Image->As<OpenGL::Texture2D>();
	image->Bind();

	ImVec2 dim = ImVec2(m_Width, m_Height);
	ImGui::SetCursorPos(ImVec2(x, y));
	ImGui::Image((void*)(uint64_t)(uint32_t)image->GetID(), dim);
}

bool Image::OnAttach() {
	if(m_Parent->GetType() == UIElement::Type::Button)
		return false; // The button captures the image

	return true;
}

bool Image::OnAddElement(Ref<UIElement> element) { return false; }

}