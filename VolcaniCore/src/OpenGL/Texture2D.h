#pragma once

#include <string>

#include <glm/vec2.hpp>

#include "Renderer/Texture.h"

namespace VolcaniCore::OpenGL {

class Texture2D : public VolcaniCore::Texture {
public:
	const uint32_t InternalFormat, DataFormat;

public:
	Texture2D(uint32_t width, uint32_t height);
	Texture2D(const std::string& path);
	Texture2D(const Texture2D& other) = default;
	~Texture2D();

	void Bind(uint32_t slot = 0);
	void SetData(const std::string& path);
	void SetData(const void* data, uint32_t size);
	void SetData(const void* data, const glm::ivec2& pos, const glm::ivec2& size);

	const std::string& GetPath() { return m_Path; }

	uint32_t GetSlot() const { return m_Slot; }
	uint32_t GetID() const { return m_TextureID; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	bool operator ==(const Texture2D& other) const { return m_TextureID == other.m_TextureID; }

private:
	uint32_t m_TextureID;
	uint32_t m_Slot = -1;
	int m_Width = 0, m_Height = 0;

	std::string m_Path;
};

}