#pragma once

#include <string>
#include <vector>

namespace VolcaniCore {

class FileUtils {
public:
	static bool FileExists(const std::string& path);

	static void CreateFile(const std::string& path);

	static std::vector<std::string> GetFiles(const std::string& dir,
											 const std::vector<std::string>&
																	extensions);

	static std::string ReadFile(const std::string& path);
	static void WriteToFile(const std::string& path,
							const std::string& info);

	static unsigned char* ReadImage(const std::string& path,
									int& width, int& height,
									int desiredChannels = 4, bool flip = 0);

	static unsigned char* ReadImage(const std::string& path,
									int& width, int& height,
									int& bitsPerPixel,
									int desiredChannels = 4,
									bool flip = 0);

	static unsigned char* ReadImage(const std::string& path,
									int desiredChannels = 4,
									bool flip = 0);

private:
	FileUtils() = delete;
	~FileUtils() = delete;
};

}