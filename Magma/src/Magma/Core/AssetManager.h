#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "Graphics/Mesh.h"

using namespace VolcaniCore;

namespace Magma {

struct Asset {
	// Type;
	// Handler;
};

class AssetManager {
public:
	AssetManager() = default;
	~AssetManager() = default;

	void Init();
	void Close();

	// template<typename T, typename ...Args>
	// Ref<T> Create(Args&&... args);

	template<typename T, typename ...Args>
	Ref<T> GetOrCreate(const Args&... args);
};

}