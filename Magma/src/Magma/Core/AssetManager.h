#pragma once

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma {

enum class AssetType { Image2D, Image3D, Mesh, Font };

struct Asset {
	const AssetType Type;
	const std::string Path;

	template<typename T>
	Ref<T> Get();
};

class AssetManager {
public:
	AssetManager() = default;
	virtual ~AssetManager() = default;

	template<typename T, typename ...Args>
	Asset GetOrCreate(Args&&... args);
};

}