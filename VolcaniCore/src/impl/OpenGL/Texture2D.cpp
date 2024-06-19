#include "Texture2D.h"

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Utils.h"

namespace VolcaniCore::OpenGL {

Texture2D::Texture2D(uint32_t width, uint32_t height)
	: VolcaniCore::Texture(width, height), InternalFormat(GL_RGBA8), DataFormat(GL_RGBA)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
	glTextureStorage2D(m_TextureID, 1, InternalFormat, m_Width, m_Height);

	glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(const std::string& path)
	: VolcaniCore::Texture(path), InternalFormat(GL_RGBA8), DataFormat(GL_RGBA)
{
	unsigned char* pixel_data = Utils::ReadImage(path.c_str(), m_Width, m_Height, 4, true);
	
	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
	glTextureStorage2D(m_TextureID, 1, InternalFormat, m_Width, m_Height);

	glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, DataFormat, GL_UNSIGNED_BYTE, pixel_data);
	stbi_image_free(pixel_data);
}

Texture2D::~Texture2D() { glDeleteTextures(1, &m_TextureID); }

void Texture2D::Bind(uint32_t slot)
{
	VOLCANICORE_ASSERT(slot >= 0);
	glBindTextureUnit(slot, m_TextureID);
}

void Texture2D::SetData(const void* data, uint32_t size)
{
	VOLCANICORE_ASSERT(size == m_Width * m_Height * 4, "Data must be the whole size of the texture.");
	glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, DataFormat, GL_UNSIGNED_BYTE, data);
}

void Texture2D::SetData(const void* data, const glm::ivec2& pos, const glm::ivec2& size)
{
	VOLCANICORE_ASSERT(pos.x + size.x <= m_Width && pos.y + size.y <= m_Height, "Offset and size must within the bounds of the texture.");
	glTextureSubImage2D(m_TextureID, 0, pos.x, pos.y, size.x, size.y, DataFormat, GL_UNSIGNED_BYTE, data);
}

}