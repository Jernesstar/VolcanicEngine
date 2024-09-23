#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include <Object/Texture.h>

#include "Object/Framebuffer.h"

namespace VolcaniCore::OpenGL {

enum class AttachmentType { Texture, RenderBuffer, None };

class Framebuffer;

class Attachment {
public:
	VolcaniCore::AttachmentTarget Target;
	AttachmentType Type;

public:
	Attachment() = default;
	Attachment(AttachmentTarget target, AttachmentType type)
		: Target(target), Type(type) { }
	~Attachment();

	void Bind(uint32_t slot = 0) const;
	uint32_t GetRendererID() const;

private:
	uint32_t m_RendererID;

	friend class Framebuffer;
};

class Framebuffer : public VolcaniCore::Framebuffer {
public:
	Framebuffer(uint32_t width, uint32_t height);
	Framebuffer(uint32_t witdth, uint32_t height,
				const std::vector<Attachment>& attachments);
	Framebuffer(AttachmentTarget target, Ref<Texture> texture);
	~Framebuffer();

	// TODO(Implement):
	void Resize(uint32_t width, uint32_t height) override { }
	void Bind() const override;
	void Unbind() const override;

	bool Has(AttachmentTarget target) const override {
		if(m_Attachments.count(target) == 1)
			return m_Attachments.at(target).Type != AttachmentType::None;

		return false;
	}

	const Attachment& Get(AttachmentTarget target) const {
		return m_Attachments.at(target);
	}

private:
	void CreateColorAttachment(Attachment& attachment);
	void CreateDepthAttachment(Attachment& attachment);
	void CreateStencilAttachment(Attachment& attachment);

	std::unordered_map<AttachmentTarget, Attachment> m_Attachments;
	uint32_t m_BufferID;
};

}