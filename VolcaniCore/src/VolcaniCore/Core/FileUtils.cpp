#include "FileUtils.h"

#include <filesystem>
#include <fstream>

#include <stb_image/stb_image.h>

#include "Core/Assert.h"

namespace VolcaniCore {

bool FileUtils::FileExists(const std::string& path) {
	return std::filesystem::exists(path.c_str());
}

void FileUtils::CreateFile(const std::string& path) {
	std::ofstream { path.c_str() };
}

std::string FileUtils::ReadFile(const std::string& file_path) {
	std::ifstream in(file_path, std::ios::in | std::ios::binary);

	VOLCANICORE_ASSERT_ARGS(in, "ReadFile: Could not open file: %s",
								file_path.c_str())

	in.seekg(0, std::ios::end); // Sets the read position at the end of the file;

	// Gets current read position. Since read position is at the end of the file
	// this operation returns the size of the file
	size_t size = in.tellg();

	VOLCANICORE_ASSERT_ARGS(size != -1, "ReadFile: Could not read from file: %s",
										file_path.c_str());

	std::string result;
	result.resize(size); // Resize the resulting string
	in.seekg(0, std::ios::beg); // Set the read position to the beginning of the file
	in.read(result.data(), size); // Read the entire file and output the contents to the string

	return result;
}

void FileUtils::WriteToFile(const std::string& path,
							const std::string& info)
{
	std::ofstream fout(path);
	fout << info.c_str();
}

unsigned char* FileUtils::ReadImage(const std::string& path,
									int& width, int& height,
									int& bitsPerPixel, int desiredChannels,
									bool flip)
{
	stbi_set_flip_vertically_on_load((int)flip);
	unsigned char* pixelData = stbi_load(path.c_str(), &width, &height,
										 &bitsPerPixel, desiredChannels);
	VOLCANICORE_ASSERT_ARGS(pixelData, "Could not load image from path %s",
										path.c_str());

	return pixelData;
}

unsigned char* FileUtils::ReadImage(const std::string& path,
									int& width, int& height,
									int desiredChannels,
									bool flip)
{
	int bitsPerPixel;
	return FileUtils::ReadImage(path, width, height, bitsPerPixel,
								desiredChannels, flip);
}

unsigned char* FileUtils::ReadImage(const std::string& path,
									int desiredChannels,
									bool flip)
{
	int width, height, bitsPerPixel;
	return FileUtils::ReadImage(path, width, height, bitsPerPixel,
								desiredChannels, flip);
}

}