#include "FileUtils.h"

#include <filesystem>
#include <fstream>

#include <stb_image/stb_image.h>

#include "Core/Assert.h"

namespace VolcaniCore {

File::File(const std::string& path)
	: m_Data(path), Path(path) { }

File& File::Write(const std::string& data) {
	m_Data << data.c_str() << "\n";
	return *this;
}

std::string File::Get() const {
	return FileUtils::ReadFile(Path);
}

bool FileUtils::FileExists(const std::string& path) {
	return std::filesystem::exists(path.c_str());
}

void FileUtils::CreateFile(const std::string& path) {
	std::ofstream{ path.c_str() };
}

std::vector<std::string> FileUtils::GetFiles(const std::string& dir)
{
	std::vector<std::string> files;
	for(auto p : std::filesystem::directory_iterator(dir.c_str()))
		files.push_back(p.path().string());

	return files;
}

std::vector<std::string> FileUtils::GetFiles(
	const std::string& dir, const std::vector<std::string>& extensions)
{
	std::vector<std::string> files;
	for(auto p : std::filesystem::directory_iterator(dir.c_str())) {
		for(auto& ext : extensions)
			if(p.path().extension().string() == ext)
				files.push_back(p.path().string());
	}
	return files;
}

std::string FileUtils::ReadFile(const std::string& filePath) {
	std::ifstream in(filePath, std::ios::in | std::ios::binary);

	VOLCANICORE_ASSERT_ARGS(in, "Could not open file: %s",
								filePath.c_str())

	// Sets the read position at the end of the file;
	in.seekg(0, std::ios::end);

	// Gets current read position. Since read position is at the end of the file
	// this operation returns the size of the file
	size_t size = in.tellg();

	VOLCANICORE_ASSERT_ARGS(size != -1, "Could not read from file '%s'",
										filePath.c_str());

	std::string str;
	str.resize(size); // Resize the resulting string
	in.seekg(0, std::ios::beg); // Set read position at the start of the file
	in.read(str.data(), size); // Read entire file, output contents to string

	return str;
}

void FileUtils::WriteToFile(const std::string& path, const std::string& info) {
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
	VOLCANICORE_ASSERT_ARGS(pixelData, "Could not load image from path '%s'",
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