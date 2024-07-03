#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) { }
	virtual ~FrameBuffer() = default;

	static Ref<FrameBuffer> Create(uint32_t width, uint32_t height);

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	uint32_t m_Width, m_Height;
};

}