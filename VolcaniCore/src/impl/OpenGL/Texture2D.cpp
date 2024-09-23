#include "Texture2D.h"

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/FileUtils.h"

namespace VolcaniCore::OpenGL {

Texture2D::Texture2D(uint32_t width, uint32_t height)
	: Texture(width, height), m_InternalFormat(RGBA8), m_ColorFormat(RGBA)
{
	m_TextureID = CreateTexture(width, height);
}

Texture2D::Texture2D(const std::string& path)
	: Texture(path), m_InternalFormat(RGBA8), m_ColorFormat(RGBA)
{
	m_TextureID = CreateTexture(m_Width, m_Height);

	unsigned char* pixelData =
		FileUtils::ReadImage(path.c_str(), m_Width, m_Height, 4, false);

	SetData(pixelData);
	stbi_image_free(pixelData);
}

Texture2D::~Texture2D() { glDeleteTextures(1, &m_TextureID); }

void Texture2D::Bind(uint32_t slot) {
	glBindTextureUnit(slot, m_TextureID);
}

void Texture2D::SetData(const void* data) {
	glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat,
						GL_UNSIGNED_BYTE, data);
}

void Texture2D::SetData(const glm::ivec2& pos, const glm::ivec2& size,
						const void* data)
{
	VOLCANICORE_ASSERT(pos.x + size.x <= m_Width && pos.y + size.y <= m_Height,
						"Coordinates must be within the bounds of the texture.");

	glTextureSubImage2D(m_TextureID, 0, pos.x, pos.y, size.x, size.y, DataFormat,
						GL_UNSIGNED_BYTE, data);
}

constexpr uint32_t GetInternalFormat(Texture::InternalFormat format) {
	switch(format) {
		case Texture::InternalFormat::Normal:
			return GL_RGBA8;
		case Texture::InternalFormat::Float:
			return GL_RGBA16F; // GL_R11F_G11F_B10F
	}
}

constexpr uint32_t GetColorFormat(Texture::ColorFormat format) {
	switch(format) {
		case Texture::ColorFormat::RGBA:
			return GL_RGBA;
		case Texture::ColorFormat::RGB:
			return GL_RGB;
		case Texture::ColorFormat::Red:
			return GL_RED;
	}
}

uint32_t CreateTexture(uint32_t width, uint32_t height,
			Texture::InternalFormat internal = Texture::InternalFormat::Normal,
			Texture::ColorFormat color       = Texture::ColorFormat::RGBA)
{
	uint32_t internalFormat, colorFormat, textureID;
	internalFormat = GetInternalFormat(internal);
	colorFormat = GetColorFormat(color);

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glTextureStorage2D(textureID, 1, internalFormat, m_Width, m_Height);

	glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;
}

}