#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Model.h>

using namespace VolcaniCore;

namespace Magma {

enum class AssetType { Mesh, Texture, Cubemap, Font, Sound };

class AssetManager;

struct Asset {
	const AssetType Type;
	const UUID ID;

	void Load();
	void Unload();

	template<typename T>
	Ref<T> Get();

private:
	bool m_Loaded = false;

	AssetManager* m_Root;

	friend class AssetManager;
};

class AssetManager {
public:
	AssetManager() = default;
	virtual ~AssetManager() = default;

	virtual void Load(Asset& asset) = 0;
	virtual void Unload(Asset& asset) = 0;

	template<typename T>
	Ref<T> Get(Asset& asset);

public:
	virtual Ref<Model> GetMesh(Asset& asset) = 0;
	virtual Ref<Texture> GetTexture(Asset& asset) = 0;
	virtual Ref<Cubemap> GetCubemap(Asset& asset) = 0;

	Map<UUID, Ref<Mesh>> m_MeshAssets;
	Map<UUID, Ref<Texture>> M_TextureAssets;
	Map<UUID, Ref<Cubemap>> m_CubemapAssets;
};

template<>
Ref<Model> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Mesh);
	return GetMesh(asset);
}

template<>
Ref<Texture> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Texture);
	return GetTexture(asset);
}

template<>
Ref<Cubemap> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Cubemap);
	return GetCubemap(asset);
}

void Asset::Load() {
	m_Root->Load(*this);
}

void Asset::Unload() {
	m_Root->Unload(*this);
}

template<typename T>
Ref<T> Asset::Get() {
	if(!m_Loaded)
		Load();

	return m_Root->Get<T>(*this);
}

}