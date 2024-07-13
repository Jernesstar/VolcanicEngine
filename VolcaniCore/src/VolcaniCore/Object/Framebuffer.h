#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {


enum class AttachmentTarget { Color, Depth, Stencil };

class Framebuffer {
public:
	static Ref<Framebuffer> Create(uint32_t width, uint32_t height);

public:
	Framebuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) { }
	virtual ~Framebuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	
	virtual bool Has(AttachmentTarget target) = 0;
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	uint32_t m_Width, m_Height;
};


}