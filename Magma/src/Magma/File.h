#pragma once

#include <string>
#include <string_view>

namespace Magma {

class File {
	static std::string OpenFile(const std::string_view& path);
	static std::string SaveFile(const std::string_view& path);
};

}