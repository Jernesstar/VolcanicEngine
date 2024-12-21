#pragma once

#include <string>

#include <glm/vec2.hpp>

#include "Graphics/Texture.h"

namespace VolcaniCore::OpenGL {

class Texture2D : public VolcaniCore::Texture {
public:
	Texture2D(const std::string& path);
	Texture2D(uint32_t id, uint32_t width, uint32_t height);
	Texture2D(uint32_t width, uint32_t height, Format format, Sampling sampling);
	~Texture2D();

	void Bind(uint32_t slot = 0);
	void SetData(const void* data);
	void SetData(const glm::ivec2& pos, const glm::ivec2& size,
				 const void* data);

	uint32_t GetID() const { return m_TextureID; }

	bool operator ==(const Texture2D& other) const {
		return this->m_TextureID == other.m_TextureID;
	}
	bool operator !=(const Texture2D& other) const {
		return this->m_TextureID != other.m_TextureID;
	}

	static uint32_t CreateTexture(uint32_t width, uint32_t height,
								  Format internal = Format::Normal,
								  Sampling sampling = Sampling::Linear);

private:
	bool m_Owns;
	uint32_t m_TextureID;
	uint32_t m_DataFormat;
};

}