#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Object/FrameBuffer.h"

namespace VolcaniCore::OpenGL {

enum class AttachmentTarget { Color, Depth, Stencil };
enum class AttachmentType	{ Texture, RenderBuffer, None };

// TODO: Have this become the base class
class Attachment {
public:
	AttachmentTarget Target;
	AttachmentType Type;

public:
	Attachment() = default;
	Attachment(AttachmentTarget target, AttachmentType type)
		: Target(target), Type(type) { }
	~Attachment();

	void Bind() const;

private:
	uint32_t m_RendererID;

	friend class FrameBuffer;
};

// TODO: Rename to Framebuffer
class FrameBuffer : public VolcaniCore::FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height);
	FrameBuffer(uint32_t witdth, uint32_t height,
				const std::vector<Attachment>& attachments);
	~FrameBuffer();

	// TODO: Possibly delete
	void Resize(uint32_t width, uint32_t height) override { }
	void Bind() const override;
	void Unbind() const override;

	bool Has(AttachmentTarget target) const {
		return m_Attachments.count(target) == 1;
	}

	const Attachment& Get(AttachmentTarget target) {
		return m_Attachments[target];
	}

private:
	void CreateColorAttachment(Attachment& attachment);
	void CreateDepthAttachment(Attachment& attachment);
	void CreateStencilAttachment(Attachment& attachment);

	std::unordered_map<AttachmentTarget, Attachment> m_Attachments;
	uint32_t m_BufferID;
};

}