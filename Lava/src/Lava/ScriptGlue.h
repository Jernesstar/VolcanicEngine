#pragma once

#include <string>

namespace Lava {

class ScriptGlue {
public:
	static void Init();
	static void Save(const std::string& path);
	static void Load(const std::string& path);
};

}