#include "Texture2D.h"

#include <glad/gl.h>

#include "Core/Assert.h"
#include "Core/FileUtils.h"

namespace VolcaniCore::OpenGL {

static constexpr uint32_t GetType(Texture::Type);
static constexpr uint32_t GetFormat(Texture::Format);

Texture2D::Texture2D(uint32_t id, uint32_t width, uint32_t height)
	: Texture(width, height), m_DataFormat(GL_RGBA)
{
	m_TextureID = id;
	m_Owns = false;
}
  
Texture2D::Texture2D(uint32_t width, uint32_t height,
					 Format format, Sampling sampling)
	: Texture(width, height), m_DataFormat(GL_RGBA)
{
	m_TextureID = CreateTexture(width, height, format, sampling);
	m_Owns = true;
}

Texture2D::~Texture2D() {
	if(m_Owns)
		glDeleteTextures(1, &m_TextureID);
}

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

	glTextureSubImage2D(m_TextureID, 0, pos.x, pos.y, size.x, size.y,
						m_DataFormat, GL_UNSIGNED_BYTE, data);
}

constexpr uint32_t GetType(Texture::Type type) {
	switch(type) {
		case Texture::Type::RGBA:
			return GL_RGBA;
		case Texture::Type::Depth:
			return GL_DEPTH_COMPONENT;
	}
}

constexpr uint32_t GetFormat(Texture::Format format) {
	switch(format) {
		case Texture::Format::Normal:
			return GL_RGBA8;
		case Texture::Format::Float:
			return GL_RGBA16F;
		case Texture::Format::Depth:
			return GL_DEPTH_COMPONENT32F;
	}

	return GL_RGBA8;
}

uint32_t Texture2D::CreateTexture(
	uint32_t width, uint32_t height, Format internalFormat, Sampling sampling)
{
	uint32_t textureID;
	auto format = GetFormat(internalFormat);
	auto filter = sampling == Sampling::Linear ? GL_LINEAR : GL_NEAREST;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glTextureStorage2D(textureID, 1, format, width, height);
	glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, filter);
	glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, filter);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;
}

}