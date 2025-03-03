#pragma once

#include <Magma/Core/AssetManager.h>

using namespace Magma;

namespace Lava {

class RuntimeAssetManager : public AssetManager {
public:
	RuntimeAssetManager();
	~RuntimeAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;

	void Load();

private:
	Map<UUID, uint64_t> m_AssetOffsets;
};

}