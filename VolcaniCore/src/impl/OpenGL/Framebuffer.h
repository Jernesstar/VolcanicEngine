#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Object/Framebuffer.h"

namespace VolcaniCore::OpenGL {

enum class AttachmentType { Texture, RenderBuffer, None };

class Attachment {
public:
	VolcaniCore::AttachmentTarget Target;
	AttachmentType Type;

public:
	Attachment() = default;
	Attachment(AttachmentTarget target, AttachmentType type)
		: Target(target), Type(type) { }
	~Attachment();

	void Bind() const;

private:
	uint32_t m_RendererID;

	friend class Framebuffer;
};

class Framebuffer : public VolcaniCore::Framebuffer {
public:
	Framebuffer(uint32_t width, uint32_t height);
	Framebuffer(uint32_t witdth, uint32_t height,
				const std::vector<Attachment>& attachments);
	~Framebuffer();

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