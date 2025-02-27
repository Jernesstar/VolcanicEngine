#include "Cubemap.h"

#include <filesystem>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>

#include "Core/FileUtils.h"
#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

Cubemap::Cubemap(const List<ImageData>& faces)
	: VolcaniCore::Cubemap()
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	for(int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
					 faces[i].Width, faces[i].Height, 0, GL_RGBA
					 GL_UNSIGNED_BYTE, faces[i].Data.Get());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap() {
	glDeleteTextures(1, &m_TextureID);
}

void Cubemap::Bind() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
}

void Cubemap::Unbind() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

}