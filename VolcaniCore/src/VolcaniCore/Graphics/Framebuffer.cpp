#include "Framebuffer.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/Framebuffer.h"
#include "OpenGL/Texture2D.h"

namespace VolcaniCore {

Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Framebuffer>(width, height);
	}

	return nullptr;
}

Ref<Framebuffer> Framebuffer::Create(
	const Map<AttachmentTarget, List<Ref<Texture>>>& textureAttachments)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
		{
			Map<AttachmentTarget, List<OpenGL::Attachment>> attachments;
			attachments.reserve(textureAttachments.size());

			for(auto& [target, textures] : textureAttachments)
				for(auto& texture : textures) {
					attachments[target]
					.Add(
						{
							OpenGL::Attachment::Type::Texture,
							texture->GetWidth(), texture->GetHeight(),
							texture->As<OpenGL::Texture2D>()->GetID()
						});

					texture->As<OpenGL::Texture2D>()->m_Owns = false;
				}

			return CreateRef<OpenGL::Framebuffer>(attachments);
		}
	}

	return nullptr;
}

}