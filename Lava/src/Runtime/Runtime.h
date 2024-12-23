#pragma once

#include <VolcaniCore/Core/Application.h>

#include <Magma/Core/DLL.h>

#include <Lava/App.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class Runtime : public Application {
public:
	Runtime(const std::string& volcPath);
	~Runtime();

	void OnUpdate(TimeStep ts) override;

	App* Get();

private:
	Ref<DLL> m_AppDLL;
};

}