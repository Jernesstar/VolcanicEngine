#pragma once

#include "Core/AssetManager.h"

namespace Magma {

class RuntimeAssetManager : public AssetManager {
public:
	RuntimeAssetManager();
	~RuntimeAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;

	void LoadRegistry(const std::string& registryFile);
};

}