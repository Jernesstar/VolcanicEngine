#pragma once

#include "Core/Template.h"
#include "Core/Defines.h"
#include "Core/Buffer.h"

namespace VolcaniCore {

struct ImageData {
	uint32_t Width, Height;
	Buffer<uint8_t> Data;
};

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
	// Texture(const ImageData& data)
	// 	: m_Width(data.Width), m_Height(data.Height) { }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	virtual void SetData(const void* data) = 0;

	template<typename T>
	void SetData(const Buffer<T>& buffer) {
		SetData(buffer.Get());
	}

protected:
	uint32_t m_Width = 0, m_Height = 0;
};

}