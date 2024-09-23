#include "Framebuffer.h"

#include <glad/glad.h>

#include "Core/Assert.h"

namespace VolcaniCore::OpenGL {

Attachment::~Attachment() {
	if(this->Type == AttachmentType::Texture)
		glDeleteTextures(1, &m_RendererID);
	else if(this->Type == AttachmentType::RenderBuffer)
		glDeleteRenderbuffers(1, &m_RendererID);
}

void Attachment::Bind(uint32_t slot) const {
	if(this->Type == AttachmentType::Texture)
		glBindTextureUnit(slot, m_RendererID);
	else if(this->Type == AttachmentType::RenderBuffer)
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}

uint32_t Attachment::GetRendererID() const {
	return m_RendererID;
}

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	: VolcaniCore::Framebuffer(width, height)
{
	glGenFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	m_Attachments.insert({ AttachmentTarget::Color,
		Attachment{ AttachmentTarget::Color, AttachmentType::Texture } });
	m_Attachments.insert({ AttachmentTarget::Depth,
		Attachment{ AttachmentTarget::Depth, AttachmentType::Texture } });

	CreateColorAttachment(m_Attachments[AttachmentTarget::Color]);
	CreateDepthAttachment(m_Attachments[AttachmentTarget::Depth]);

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(uint32_t width, uint32_t height,
						 const std::vector<Attachment>& attachments)
	: VolcaniCore::Framebuffer(width, height)
{
	glGenFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	for(auto& attachment : attachments)
		m_Attachments.insert({ attachment.Target, attachment });

	for(auto& [target, att] : m_Attachments) {
		if(target == AttachmentTarget::Color)	CreateColorAttachment(att);
		if(target == AttachmentTarget::Depth)	CreateDepthAttachment(att);
		if(target == AttachmentTarget::Stencil) CreateStencilAttachment(att);
	}

	if(!Has(AttachmentTarget::Color)) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		m_Attachments.erase(AttachmentTarget::Color);
	}
	if(!Has(AttachmentTarget::Depth))
		m_Attachments.erase(AttachmentTarget::Depth);
	if(!Has(AttachmentTarget::Stencil))
		m_Attachments.erase(AttachmentTarget::Stencil);

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::CreateColorAttachment(Attachment& attachment) {
	if(attachment.Type == AttachmentType::Texture) {
		glGenTextures(1, &attachment.m_RendererID);
		// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, attachment.m_RendererID);
		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);

		uint32_t internalFormat = GL_RGBA16F;
		uint32_t colorFormat    = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0,
					 colorFormat, GL_UNSIGNED_BYTE, NULL);
		// uint32_t samples = 4;
		// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, m_Width, m_Height, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);\
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, attachment.m_RendererID, 0);
	}
	else if(attachment.Type == AttachmentType::RenderBuffer) {
		// TODO(Implement):
	}
}

void Framebuffer::CreateDepthAttachment(Attachment& attachment) {
	if(attachment.Type == AttachmentType::Texture) {
		glGenTextures(1, &attachment.m_RendererID);
		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0,
					 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D, attachment.m_RendererID, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if(attachment.Type == AttachmentType::RenderBuffer)
	{
		if(this->Has(AttachmentTarget::Stencil)) {
			auto& stencilAttachment = this->Get(AttachmentTarget::Stencil);

			if(stencilAttachment.Type == AttachmentType::RenderBuffer) {
				glGenRenderbuffers(1, &attachment.m_RendererID);
				glBindRenderbuffer(GL_RENDERBUFFER, attachment.m_RendererID);

				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
									  m_Width, m_Height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER,
										  GL_DEPTH_STENCIL_ATTACHMENT,
										  GL_RENDERBUFFER,
										  attachment.m_RendererID);
			}
		}
		else {
			// TODO(Implement):
		}
	}
}

void Framebuffer::CreateStencilAttachment(Attachment& attachment) {
	// TODO(Implement):
}

Framebuffer::~Framebuffer() {
	glDeleteBuffers(1, &m_BufferID);
}

void Framebuffer::Bind() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_BufferID);
}

void Framebuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}