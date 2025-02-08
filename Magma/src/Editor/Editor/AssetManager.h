#pragma once

#include "Core/AssetManager.h"

namespace Magma {

class EditorAssetManager : public AssetManager {
public:
	EditorAssetManager();
	~EditorAssetManager();

	void Load(Asset& asset) override;
	void Unload(Asset& asset) override;

	void AddMesh(Ref<Model> mesh);

	void Save(const std::string& path);
	void SaveRuntime(const std::string& path);
};

}