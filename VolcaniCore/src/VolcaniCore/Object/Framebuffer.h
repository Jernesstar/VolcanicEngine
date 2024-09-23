#pragma once

#include "Core/Defines.h"

#include <Object/Texture.h>

namespace VolcaniCore {

enum class AttachmentTarget { Color, Depth, Stencil };

class Framebuffer {
public:
	static Ref<Framebuffer> Create(uint32_t width, uint32_t height);
	static Ref<Framebuffer> Create(
		const Map<AttachmentTarget, List<Ref<Texture>>>& textureAttachments);

public:
	Framebuffer() = default;
	Framebuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) { }
	virtual ~Framebuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual bool Has(AttachmentTarget target) const = 0;
	virtual void Set(AttachmentTarget target, Ref<Texture> texture,
					 uint32_t index = 0) const = 0;
	// virtual void Bind(AttachmentTarget target, uint32_t slot,
	// 				  uint32_t index = 0) const = 0;

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Framebuffer>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	uint32_t m_Width, m_Height;
};

}