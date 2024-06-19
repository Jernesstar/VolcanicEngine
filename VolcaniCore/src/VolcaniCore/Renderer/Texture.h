#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Texture {
public:
	const std::string Path;

public:
	Texture(uint32_t width, uint32_t height) { m_Width = width; m_Height = height; }
	Texture(const std::string& path) : Path(path) { }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	static Ref<Texture> Create(uint32_t width, uint32_t height);
	static Ref<Texture> Create(const std::string& path);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	uint32_t m_Width, m_Height;
};

}