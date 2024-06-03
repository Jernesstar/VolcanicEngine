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

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

public:
	uint32_t m_Width, m_Height;
};

}