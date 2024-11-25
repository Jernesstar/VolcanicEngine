#include "Image.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "VolcaniCore/Core/Log.h"

#include <OpenGL/Framebuffer.h>
#include <OpenGL/Texture2D.h>

using namespace VolcaniCore;

namespace Magma::UI {

Ref<UI::Image> Image::Create(const UI::Image::Specification& specs) {
	Ref<UI::Image> image;
	if(specs.Path != "")
		image = CreateRef<UI::Image>(specs.Path);
	else
		image = CreateRef<UI::Image>(Texture::Create(specs.Width, specs.Height));

	image->SetPosition(specs.x, specs.y);
	if(specs.Width && specs.Height)
		image->SetSize(specs.Width, specs.Height);
	return image;
}

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
	auto& attachment = framebuffer->As<OpenGL::Framebuffer>()->Get(target);

	Content->As<OpenGL::Texture2D>()->SetID(attachment.GetRendererID());
	Width = attachment.GetWidth();
	Height = attachment.GetHeight();
}

void Image::Draw() {
	auto image = Content->As<OpenGL::Texture2D>();

	auto v0 = ImVec2(0.0f, 1.0f);
	auto v1 = ImVec2(1.0f, 0.0f);
	ImVec2 dim = ImVec2(Width, Height);
	ImGui::SetCursorPos(ImVec2(x, y));
	ImGui::Image((ImTextureID)(intptr_t)image->GetID(), dim, v0, v1);
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