#include "FrameBuffer.h"

#include <glad/glad.h>

#include "Core/Assert.h"

namespace VolcaniCore::OpenGL {

Attachment::~Attachment() {
	if(this->Type == AttachmentType::Texture)
		glDeleteTextures(1, &m_RendererID);
	else if(this->Type == AttachmentType::RenderBuffer)
		glDeleteRenderbuffers(1, &m_RendererID);
}

void Attachment::Bind() const {
	if(Type == AttachmentType::Texture)
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	else if(Type == AttachmentType::RenderBuffer)
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}


FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
	: VolcaniCore::FrameBuffer(width, height)
{

}

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height,
						 const std::vector<Attachment>& attachments)
	: VolcaniCore::FrameBuffer(width, height)
{
	glGenFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	for(auto& attachment : attachments)
		m_Attachments.insert({ attachment.Target, attachment });

	for(auto& [_, attachment] : m_Attachments) {
		if(attachment.Target == AttachmentTarget::Color)
			CreateColorAttachment(attachment);
		if(attachment.Target == AttachmentTarget::Depth)
			CreateDepthAttachment(attachment);
		if(attachment.Target == AttachmentTarget::Stencil)
			CreateStencilAttachment(attachment);
	}

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CreateColorAttachment(Attachment& attachment) {
	if(attachment.Type == AttachmentType::Texture) {
		glGenTextures(1, &attachment.m_RendererID);

		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB,
					 GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}
	else if(attachment.Type == AttachmentType::RenderBuffer) {
		// TODO: Implement
	}
	else if(attachment.Type == AttachmentType::None) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
}

void FrameBuffer::CreateDepthAttachment(Attachment& attachment) {
	if(attachment.Type == AttachmentType::RenderBuffer)
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
			// TODO: Implement
		}
	}
	if(attachment.Type == AttachmentType::Texture) {
		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 m_Width, m_Height, 0, GL_DEPTH_COMPONENT,
					 GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

void FrameBuffer::CreateStencilAttachment(Attachment& attachment) {
	// TODO: Implement
}

FrameBuffer::~FrameBuffer() {
	glDeleteBuffers(1, &m_BufferID);
}

void FrameBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
}
void FrameBuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


}