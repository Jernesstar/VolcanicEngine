#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Object/FrameBuffer.h"

namespace VolcaniCore::OpenGL {

enum class AttachmentTarget { Color, Depth, Stencil };
enum class AttachmentType	{ Texture, RenderBuffer, None };

class Attachment {
public:
	const AttachmentTarget Target;
	const AttachmentType Type;

	void Bind() const {
		if(Type == AttachmentType::Texture)
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
		else if(Type == Attachment::RenderBuffer)
			glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	}

private:
	uint32_t m_RendererID;

	friend class FrameBuffer;
};

class FrameBuffer : public VolcaniCore::FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height);
	FrameBuffer(uint32_t witdth, uint32_t height,
				const std::std::initializer_list<Attachment>& attachments);
	~FrameBuffer();

	void Resize(uint32_t width, uint32_t height) override;
	void Bind() const override;
	void Unbind() const override;

	bool Has(AttachmentTarget target) const {
		return m_Components.count(target) == 1;
	}

	const Attachment& Get(AttachmentTarget) const {
		return m_Attachments[target];
	}

private:
	void CreateColorAttachment(const Attachment& attachment);
	void CreateDepthAttachment(const Attachment& attachment);
	void CreateStencilAttachment(const Attachment& attachment);

	std::unordered_map<AttachmentTarget, Attachment> m_Attachments;
};

}