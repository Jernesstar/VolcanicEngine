#pragma once

namespace VolcaniCore {

class Texture {
public:
	Texture(const std::string& path);

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	static Ref<Texture> Create(const std::string& path);

protected:
	uint32_t m_Width, m_Height;
};

}