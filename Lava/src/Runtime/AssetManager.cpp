#include "AssetManager.h"

#include <Magma/Core/BinaryReader.h>

namespace Lava {

RuntimeAssetManager::RuntimeAssetManager() {

}

RuntimeAssetManager::~RuntimeAssetManager() {

}

void RuntimeAssetManager::Load(Asset asset) {

}

void RuntimeAssetManager::Unload(Asset asset) {

}

void RuntimeAssetManager::Load() {
	BinaryReader reader("./.volc.assetpk");

	
}

}