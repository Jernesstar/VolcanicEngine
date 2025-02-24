#pragma once

#include "Core/Defines.h"
#include "Core/Template.h"

namespace VolcaniCore {

class Texture : public Derivable<Texture> {
public:
	enum class Type { RGBA, Depth, Stencil };
	enum class Format { Normal, Float, Depth };
	enum class Sampling { Nearest, Linear };

public:
	static Ref<Texture> Create(uint32_t width, uint32_t height,
		Format format = Format::Normal,
		Sampling sampling = Sampling::Linear);

public:
	Texture() = default;
	Texture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) { }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	virtual void SetData(const void* data) = 0;

protected:
	uint32_t m_Width = 0, m_Height = 0;
};

}