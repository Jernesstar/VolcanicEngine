#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace VolcaniCore {

class File {
public:
	const std::string Path;

public:
	File(const std::string& path);

	File& Write(const std::string& info);

	std::string Get() const;

	void Close();

private:
	std::ofstream m_Data;
};

class FileUtils {
public:
	static bool FileExists(const std::string& path);

	static void CreateFile(const std::string& path);

	static std::string ReadFile(const std::string& path);

	static void WriteToFile(const std::string& path, const std::string& info);

	static std::vector<std::string> GetFiles(const std::string& dir);
	static std::vector<std::string> GetFiles(
		const std::string& dir, const std::vector<std::string>& ext);
};

}