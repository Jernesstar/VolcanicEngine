#pragma once

#include <Core/Defines.h>

#include <Object/Framebuffer.h>

namespace VolcaniCore::OpenGL {

class Framebuffer;

class Attachment {
public:
	enum class Type { Texture, RenderBuffer };

public:
	Attachment() = default;
	Attachment(Attachment::Type type, uint32_t id = 0)
		: Type(type), m_RendererID(id) { }
	~Attachment();

	void Bind(uint32_t slot = 0) const;
	uint32_t GetRendererID() const;

	Attachment::Type GetType() const { return m_Type; }

private:
	Attachment::Type m_Type;
	uint32_t m_RendererID;

	friend class Framebuffer;
};

class Framebuffer : public VolcaniCore::Framebuffer {
public:
	Framebuffer(uint32_t width, uint32_t height);
	Framebuffer(uint32_t witdth, uint32_t height,
				const Map<AttachmentTarget, List<Attachment>>& attachmentMap);
	~Framebuffer();

	void Bind() const override;
	void Unbind() const override;

	bool Has(AttachmentTarget target) const override {
		return m_AttachmentMap.count(target) == 1;
	}
	void Set(AttachmentTarget target,
			 Ref<Texture> texture, uint32_t index = 0) const override;

	const Attachment& Get(AttachmentTarget target, uint32_t index = 0) const {
		return m_AttachmentMap.at(target).at(index);
	}

private:
	void CreateColorAttachment(Attachment& attachment, uint32_t index);
	void CreateDepthAttachment(Attachment& attachment);
	void CreateStencilAttachment(Attachment& attachment);

	Map<AttachmentTarget, List<Attachment>> m_AttachmentMap;
	uint32_t m_BufferID;
};

}