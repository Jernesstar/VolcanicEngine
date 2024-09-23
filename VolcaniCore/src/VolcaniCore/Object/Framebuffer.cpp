#include "Framebuffer.h"

#include "OpenGL/Framebuffer.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height) {
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Framebuffer>(width, height);
	}
}

Ref<Framebuffer> Framebuffer::Create(
	const Map<AttachmentTarget, List<Ref<Texture>>>& textureAttachments)
{
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
		{
			Map<AttachmentTarget, List<OpenGL::Attachment>> attachments;
			attachments.reserve(textureAttachments.size());

			for(const auto& [target, textures] : textureAttachments) {
				for(const auto& texture : textures) {
					attachments[target].push_back(
					{
						OpenGL::Attachment::Type::Texture,
						texture->As<OpenGL::Texture2D>()->GetID()
					});
				}
			}

			return CreateRef<OpenGL::Framebuffer>(attachments);
		}
	}
}

}