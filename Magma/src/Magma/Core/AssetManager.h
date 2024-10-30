#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "Object/Mesh.h"

using namespace VolcaniCore;

namespace Magma {

class AssetManager {
public:
	AssetManager() = delete;
	~AssetManager() = delete;

	static void Init();
	static void Close();

	// template<typename T, typename ...Args>
	// static Ref<T> Create(Args&&... args);

	template<typename T, typename ...Args>
	static Ref<T> CreateOrReturn(const Args&... args);
};

}