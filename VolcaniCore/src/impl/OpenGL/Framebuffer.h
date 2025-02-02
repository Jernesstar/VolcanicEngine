#pragma once

#include <Core/Defines.h>
#include <Core/List.h>

#include <Graphics/Framebuffer.h>

#include <OpenGL/Texture2D.h>

namespace VolcaniCore::OpenGL {

class Attachment {
public:
	enum class Type { Texture, RenderBuffer };

public:
	Attachment(Attachment::Type type, uint32_t width = 0, uint32_t height = 0,
				uint32_t id = 0)
		: m_Type(type), m_Width(width), m_Height(height), m_RendererID(id) { }

	void Bind(uint32_t slot = 0) const;

	Attachment::Type GetType() const { return m_Type; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	uint32_t GetRendererID() const { return m_RendererID; };

private:
	void Delete() const;

	Attachment::Type m_Type;
	uint32_t m_Width, m_Height;
	uint32_t m_RendererID;

	Ref<Texture2D> m_Texture;

	friend class Framebuffer;
};

class Framebuffer : public VolcaniCore::Framebuffer {
public:
	Framebuffer(uint32_t width, uint32_t height);
	Framebuffer(const Map<AttachmentTarget, List<Attachment>>& map);
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	bool Has(AttachmentTarget target) const override {
		return m_AttachmentMap.count(target) == 1;
	}
	void Add(AttachmentTarget target, Ref<Texture> texture) override;
	void Attach(AttachmentTarget target, uint32_t idx, uint32_t dst) override;
	Ref<Texture> Get(AttachmentTarget target, uint32_t idx = 0) const override;

	void Bind(AttachmentTarget target, uint32_t slot, uint32_t index = 0) const;

	const Attachment& GetAttachment(AttachmentTarget target,
									uint32_t index = 0) const
	{
		return m_AttachmentMap.at(target)[index];
	}

private:
	void CreateColorAttachment(uint32_t index = 0);
	void CreateDepthAttachment();
	void CreateStencilAttachment();
	void CreateDepthStencilAttachment();

	Map<AttachmentTarget, List<Attachment>> m_AttachmentMap;
	uint32_t m_BufferID;
};

}