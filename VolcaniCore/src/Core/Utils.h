#pragma once

#include <string>

namespace VolcaniCore {

class Utils {
public:
	static std::string ReadFile(const std::string& file_path);

	static unsigned char* ReadImage(const std::string& path, int& width, int& height, 
		int desired_channels = 4, bool flip = 0);

	static unsigned char* ReadImage(const std::string& path, int& width, int& height,
		int& bits_per_pixel, int desired_channels = 4, bool flip = 0);

	static unsigned char* ReadImage(const std::string& path, int desired_channels = 4, bool flip = 0);

private:
	Utils() = delete;
	~Utils() = delete;
};

}