#include "Framebuffer.h"

#include <algorithm>

#include <glad/glad.h>

#include "Core/Assert.h"

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

void Attachment::Delete() const {
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

	uint32_t atts[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, atts);

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(const Map<AttachmentTarget, List<Attachment>>& map,
						 uint32_t width, uint32_t height)
	: VolcaniCore::Framebuffer(width, height), m_AttachmentMap(map)
{
	if(m_Width == 0 || m_Height == 0) {
		if(Has(AttachmentTarget::Color)) {
			auto& att = m_AttachmentMap[AttachmentTarget::Color][0];
			m_Width  = att.m_Width;
			m_Height = att.m_Height;
		}
		else
			for(const auto& [_, attachments] : m_AttachmentMap)
				for(const auto& att : attachments) {
					m_Width  = std::max(m_Width, att.m_Width);
					m_Height = std::max(m_Height, att.m_Height);
				}
	}

	glGenFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	if(!Has(AttachmentTarget::Color)) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	// TODO(Change): Replace with RenderBuffer
	if(!Has(AttachmentTarget::Depth))
		m_AttachmentMap.insert(
		{
			AttachmentTarget::Depth, { { Attachment::Type::Texture } }
		});

	uint32_t index = 0;
	for(auto& _ : m_AttachmentMap[AttachmentTarget::Color])
		CreateColorAttachment(index++);

	CreateDepthAttachment();

	// // TODO(Fix): Don't create if already have Depth-Stencil buffer
	// if(Has(AttachmentTarget::Stencil))
	// 	CreateStencilAttachment();

	if(index != 0) {
		uint32_t* atts = new uint32_t[index];
		for(uint32_t i = 0; i < index; i++)
			atts[i] = GL_COLOR_ATTACHMENT0 + i;

		glDrawBuffers(index, atts);
		delete atts;
	}

	VOLCANICORE_ASSERT(
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
	for(const auto& [target, attachments] : m_AttachmentMap)
		for(const auto& att : attachments)
			att.Delete();

	glDeleteBuffers(1, &m_BufferID);
}

void Framebuffer::Bind() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_BufferID);
}

void Framebuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Set(AttachmentTarget target, Ref<Texture> texture,
						uint32_t index)
{
	if(!this->Has(target)) {
		VOLCANICORE_LOG_WARNING("Need attachemnt not found");
		return;
	}

	uint32_t id = texture->As<OpenGL::Texture2D>()->GetID();
	uint32_t type;
	switch(target) {
		case AttachmentTarget::Color:
			type = GL_COLOR_ATTACHMENT0;
			break;
		case AttachmentTarget::Depth:
			type = GL_DEPTH_ATTACHMENT;
			index = 0;
			break;
		case AttachmentTarget::Stencil:
			type = GL_STENCIL_ATTACHMENT;
			index = 0;
			break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, type + index, GL_TEXTURE_2D, id, 0);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto& att = m_AttachmentMap[target][index];
	att.m_RendererID = id;
	att.m_Width = texture->GetWidth();
	att.m_Height = texture->GetHeight();
	m_Width = att.m_Width;
	m_Height = att.m_Height;
}

void Framebuffer::Bind(AttachmentTarget target, uint32_t slot,
						uint32_t index) const
{
	Get(target, index).Bind(slot);
}

void Framebuffer::CreateColorAttachment(uint32_t index) {
	auto& attachment = m_AttachmentMap[AttachmentTarget::Color][index];
	uint32_t& width  = attachment.m_Width;
	uint32_t& height = attachment.m_Height;
	width  = width  ? width  : m_Width;
	height = height ? height : m_Height;

	if(attachment.GetType() == Attachment::Type::Texture) {
		if(attachment.m_RendererID == 0) {
			Texture::InternalFormat format = Texture::InternalFormat::Float;
			uint32_t id = Texture2D::CreateTexture(width, height, format);
			attachment.m_RendererID = id;
		}

		// uint32_t samples = 4;
		// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, m_Width, m_Height, GL_TRUE);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, attachment.m_RendererID, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}
	else if(attachment.GetType() == Attachment::Type::RenderBuffer) {
		// TODO(Implement):
	}
}

void Framebuffer::CreateDepthAttachment() {
	auto& attachment = m_AttachmentMap[AttachmentTarget::Depth][0];
	uint32_t& width  = attachment.m_Width;
	uint32_t& height = attachment.m_Height;
	width  = width  ? width  : m_Width;
	height = height ? height : m_Height;

	if(attachment.GetType() == Attachment::Type::Texture) {
		if(attachment.m_RendererID == 0) {
			Texture::InternalFormat format = Texture::InternalFormat::Depth;
			uint32_t id = Texture2D::CreateTexture(width, height, format);
			attachment.m_RendererID = id;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								GL_TEXTURE_2D, attachment.m_RendererID, 0);
	}
	else if(attachment.GetType() == Attachment::Type::RenderBuffer) {
		if(this->Has(AttachmentTarget::Stencil)) {
			auto& stencilAttachment = this->Get(AttachmentTarget::Stencil);

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
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
							  GL_RENDERBUFFER, depthAttachment.m_RendererID);
}

}