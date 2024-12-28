#pragma once

#include <Magma/Core/Project.h>
#include <Magma/Core/DLL.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class ProjectLoader {
public:
	static void Load(Project& proj, const std::string& volcPath);
	static void Save(const Project& proj, const std::string& volcPath);
	static void Compile(const std::string& volcPath);
	static Ref<DLL> GetDLL();
};

}