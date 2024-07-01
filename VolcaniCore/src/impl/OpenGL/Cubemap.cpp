#include "Cubemap.h"

#include <filesystem>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Core/FileUtils.h"
#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

std::vector<std::filesystem::path> GetImagePaths(const std::string& folder)
{
	std::vector<std::filesystem::path> paths;
	for(const auto& path : FileUtils::GetFiles(""))
	{
		paths.push_back(path);

		if(paths.size() == 6)
			break;
	}

	if(paths.size() < 6)
		VOLCANICORE_LOG_WARNING(
			"Cubemap folder %s does not have at least 6 images",
			folder.c_str());
	return paths;
}

Cubemap::Cubemap(const std::string& cubemap_folder)
	: VolcaniCore::Cubemap(cubemap_folder)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	std::vector<std::filesystem::path> faces = GetImagePaths(cubemap_folder);
	std::unordered_map<std::string, int> map =
	{
		{ "right", 0 }, { "left", 1 },
		{ "top", 2 }, { "bottom", 3 },
		{ "front", 4 }, { "back", 5 }
	};

	for(auto& face : faces)
	{
		int width, height, bpp;
		unsigned char* data = FileUtils::ReadImage(face.string(), width, height,
												   bpp, 0, false);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + map[face.stem().string()],
					 0, GL_RGB, width, height, 0,
					 GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::Cubemap(const std::vector<std::string>& faces)
	: VolcaniCore::Cubemap(faces)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	for(int i = 0; i < 6; i++)
	{
		int width, height, bpp;
		unsigned char* data = FileUtils::ReadImage(faces[i], width, height, bpp,
												   0, false);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
					 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::Bind() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
}

}