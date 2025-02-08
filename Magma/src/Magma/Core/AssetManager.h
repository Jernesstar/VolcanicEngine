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
	AssetType Type;
	UUID ID;

	void Load();
	void Unload();

	template<typename T>
	Ref<T> Get();

private:
	AssetManager* m_Root;

	friend class AssetManager;
};

class AssetManager {
public:
	AssetManager() = default;
	virtual ~AssetManager() = default;

	bool IsLoaded(Asset& asset) {
		return m_AssetRegistry[asset.ID];
	}
	virtual void Load(Asset& asset) = 0;
	virtual void Unload(Asset& asset) = 0;

	template<typename T>
	Ref<T> Get(Asset& asset);

protected:
	Map<UUID, bool> m_AssetRegistry;
	Map<UUID, Ref<Model>> m_MeshAssets;
	Map<UUID, Ref<Texture>> m_TextureAssets;
	Map<UUID, Ref<Cubemap>> m_CubemapAssets;

private:
	Ref<Model> GetMesh(Asset& asset) {
		return m_MeshAssets[asset.ID];
	}
	Ref<Texture> GetTexture(Asset& asset) {
		return m_TextureAssets[asset.ID];
	}
	Ref<Cubemap> GetCubemap(Asset& asset) {
		return m_CubemapAssets[asset.ID];
	}
};

template<> inline
Ref<Model> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Mesh);
	return GetMesh(asset);
}

template<> inline
Ref<Texture> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Texture);
	return GetTexture(asset);
}

template<> inline
Ref<Cubemap> AssetManager::Get(Asset& asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Cubemap);
	return GetCubemap(asset);
}

inline void Asset::Load() {
	m_Root->Load(*this);
}

inline void Asset::Unload() {
	m_Root->Unload(*this);
}

template<typename T> inline
Ref<T> Asset::Get() {
	if(!m_Root->IsLoaded(*this))
		Load();

	return m_Root->Get<T>(*this);
}

}