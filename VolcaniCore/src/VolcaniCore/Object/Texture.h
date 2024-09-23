#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Texture {
public:
	enum class InternalFormat { Normal, Float };
enum class ColorFormat { Red, RGB, RGBA };

public:
	static Ref<Texture> Create(uint32_t width, uint32_t height);
	static Ref<Texture> Create(const std::string& path);

public:
	Texture() = default;
	Texture(const std::string& path)
		: m_Path(path) { }
	Texture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) { }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	bool HasPath() const { return m_Path != ""; }
	const std::string& GetPath() const { return m_Path; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Texture>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	std::string m_Path = "";
	uint32_t m_Width, m_Height;
};

}