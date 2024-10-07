#include "Texture2D.h"

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/FileUtils.h"

namespace VolcaniCore::OpenGL {

Texture2D::Texture2D(uint32_t width, uint32_t height,
					 InternalFormat format, SamplingOption sampling)
	: Texture(width, height), m_DataFormat(GL_RGBA)
{
	m_TextureID = CreateTexture(width, height, format, sampling);
}

Texture2D::Texture2D(const std::string& path)
	: Texture(path), m_DataFormat(GL_RGBA)
{
	int32_t width, height;
	unsigned char* pixelData = FileUtils::ReadImage(path, width, height, 4);
	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	m_TextureID = CreateTexture(m_Width, m_Height);

	SetData(pixelData);

	stbi_image_free(pixelData);
}

Texture2D::~Texture2D() {
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

// constexpr uint32_t GetType(Texture::Type format) {
// 	switch(format) {
// 		case Texture::Type::Depth:
// 			return GL_DEPTH_COMPONENT;
// 		case Texture::Type::RGB:
// 			return GL_RGB;
// 		case Texture::Type::RGBA:
// 			return GL_RGBA;
// 	}
// }

constexpr uint32_t GetInternalFormat(Texture::InternalFormat format) {
	switch(format) {
		case Texture::InternalFormat::Normal:
			return GL_RGBA8;
		case Texture::InternalFormat::Float:
			return GL_RGBA16F; // GL_R11F_G11F_B10F
		case Texture::InternalFormat::Depth:
			return GL_DEPTH_COMPONENT32F;
	}
}

uint32_t Texture2D::CreateTexture(uint32_t width, uint32_t height,
									InternalFormat internal,
									SamplingOption sampling)
{
	uint32_t textureID;
	auto internalFormat = GetInternalFormat(internal);
	auto filter = sampling == SamplingOption::Linear ? GL_LINEAR : GL_NEAREST;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glTextureStorage2D(textureID, 1, internalFormat, width, height);
	glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, filter);
	glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, filter);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;
}

}