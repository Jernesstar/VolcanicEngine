#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Texture {
public:
	Texture() = default;
	Texture(uint32_t width, uint32_t height) {
		m_Width = width;
		m_Height = height;
	}

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	bool HasPath() const { return m_Path != ""; }
	std::string GetPath() const { return m_Path; }

	static Ref<Texture> Create(uint32_t width, uint32_t height);
	static Ref<Texture> Create(const std::string& path);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	std::string m_Path;
	uint32_t m_Width, m_Height;
};

}