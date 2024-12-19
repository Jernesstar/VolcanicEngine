#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Texture {
public:
	// enum class Type { Depth, RGB, RGBA };
	enum class Format { Normal, Float, Depth };
	enum class Sampling { Nearest, Linear };

public:
	static Ref<Texture> Create(uint32_t width, uint32_t height,
		Format format = Format::Normal,
		Sampling sampling = Sampling::Linear);
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