#include "Image.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "VolcaniCore/Core/Log.h"

#include <OpenGL/Framebuffer.h>
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

}

}