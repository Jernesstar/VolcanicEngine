#pragma once

#include <VolcaniCore/Core/Application.h>

#include <Magma/Core/DLL.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class Runtime : public Application {
public:
	Runtime(const std::string& volcPath);
	~Runtime();

	void OnUpdate(TimeStep ts) override;

private:
	void Load();
	void Reload();

	std::string m_Name;
	std::string m_RootPath;
	std::string m_SrcPath;
	std::string m_AssetPath;
 
	Ref<DLL> m_GameDLL;
};

}