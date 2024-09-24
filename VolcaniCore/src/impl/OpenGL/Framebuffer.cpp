#include "Framebuffer.h"

#include <algorithm>

#include <glad/glad.h>

#include "Core/Assert.h"

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

Attachment::~Attachment() {
	if(this->GetType() == Attachment::Type::Texture)
		glDeleteTextures(1, &m_RendererID);
	else if(this->GetType() == Attachment::Type::RenderBuffer)
		glDeleteRenderbuffers(1, &m_RendererID);
}

void Attachment::Bind(uint32_t slot) const {
	if(this->GetType() == Attachment::Type::Texture)
		glBindTextureUnit(slot, m_RendererID);
	else if(this->GetType() == Attachment::Type::RenderBuffer)
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

	// TODO(Change): Renderbuffer
	m_AttachmentMap.insert(
	{
		AttachmentTarget::Color, { { Attachment::Type::Texture } }
	});
	m_AttachmentMap.insert(
	{
		AttachmentTarget::Depth, { { Attachment::Type::Texture } }
	});

	CreateColorAttachment();
	CreateDepthAttachment();

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(const Map<AttachmentTarget, List<Attachment>>& map,
						 uint32_t width, uint32_t height)
	: VolcaniCore::Framebuffer(width, height), m_AttachmentMap(map)
{
	if(m_Width == 0 || m_Height == 0)
		for(const auto& [_, attachments] : m_AttachmentMap)
			for(const auto& att : attachments) {
				m_Width  = std::max(m_Width, att.m_Width);
				m_Height = std::max(m_Height, att.m_Height);
			}

	glGenFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	if(!Has(AttachmentTarget::Depth)) {
		m_AttachmentMap.insert(
		{
			// TODO(Change): Replace with RenderBuffer
			AttachmentTarget::Depth, { { Attachment::Type::Texture } }
		});
	}

	uint32_t index = 0;
	for(auto& _ : m_AttachmentMap[AttachmentTarget::Color])
		CreateColorAttachment(index++);
	
	CreateDepthAttachment();

	if(Has(AttachmentTarget::Stencil))
		CreateStencilAttachment();

	if(!Has(AttachmentTarget::Color)) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	uint32_t* atts = new uint32_t[index];
	for(uint32_t i = 0; i < index; i++)
		atts[i] = GL_COLOR_ATTACHMENT0 + i;

	glDrawBuffers(index, atts);
	delete atts;

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Set(AttachmentTarget target, Ref<Texture> texture,
						uint32_t index) const
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
		GL_TEXTURE_2D, texture->As<OpenGL::Texture2D>()->GetID(), 0);
}

void Framebuffer::Bind(AttachmentTarget target, uint32_t slot,
						uint32_t index) const
{
	Get(target, index).Bind(slot);
}

void Framebuffer::CreateColorAttachment(uint32_t index) {
	auto& attachment = m_AttachmentMap[AttachmentTarget::Color][index];

	// if(attachment.m_RendererID != 0) {
	// 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
	// 							GL_TEXTURE_2D, attachment.m_RendererID, 0);
	// }

	if(attachment.GetType() == Attachment::Type::Texture) {
		glGenTextures(1, &attachment.m_RendererID);
		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);

		uint32_t internalFormat = GL_RGBA16F;
		uint32_t colorFormat    = GL_RGBA;
		// uint32_t internalFormat = GL_RGB;
		// uint32_t colorFormat    = GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0,
					 colorFormat, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Texture::InternalFormat internalFormat = Texture::InternalFormat::Float;
		// Texture::ColorFormat colorFormat = Texture::ColorFormat::RGBA;
		// attachment.m_RendererID =
		// 	OpenGL::Texture::CreateTexture(m_Width, m_Height, internalFormat);

		// uint32_t samples = 4;
		// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, m_Width, m_Height, GL_TRUE);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, attachment.m_RendererID, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}
	else if(attachment.GetType() == Attachment::Type::RenderBuffer) {
		// TODO(Implement):
	}
}

void Framebuffer::CreateDepthAttachment() {
	auto& attachment = m_AttachmentMap[AttachmentTarget::Depth][0];

	if(attachment.m_RendererID != 0) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}

	if(attachment.GetType() == Attachment::Type::Texture) {
		glGenTextures(1, &attachment.m_RendererID);
		glBindTexture(GL_TEXTURE_2D, attachment.m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0,
					 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}
	else if(attachment.GetType() == Attachment::Type::RenderBuffer) {
		if(this->Has(AttachmentTarget::Stencil)) {
			const auto& stencilAttachment = this->Get(AttachmentTarget::Stencil);

			if(stencilAttachment.GetType() == Attachment::Type::RenderBuffer) {
				CreateDepthStencilAttachment();
				return;
			}
		}

		// TODO(Implement):
	}
}

void Framebuffer::CreateStencilAttachment() {
	// TODO(Implement):
}

void Framebuffer::CreateDepthStencilAttachment() {
	auto& depthAttachment = m_AttachmentMap[AttachmentTarget::Depth][0];
	auto& stencilAttachment = m_AttachmentMap[AttachmentTarget::Stencil][0];

	glGenRenderbuffers(1, &depthAttachment.m_RendererID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment.m_RendererID);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
							m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
								GL_DEPTH_STENCIL_ATTACHMENT,
								GL_RENDERBUFFER,
								depthAttachment.m_RendererID);
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