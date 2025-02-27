#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Mesh.h>

#include <Magma/Audio/Sound.h>

using namespace VolcaniCore;

namespace Magma {

enum class AssetType { Mesh, Texture, Cubemap, Font, Audio, Shader, None };

struct Asset {
	UUID ID = 0;
	AssetType Type = AssetType::None;
	bool Primary = true;

	operator uint64_t() const { return ID; }
};

}


namespace std {

template<>
struct hash<Magma::Asset> {
	std::size_t operator()(const Magma::Asset& asset) const {
		return (uint64_t)asset;
	}
};

}


namespace Magma {

class AssetManager {
public:
	AssetManager() = default;
	virtual ~AssetManager() = default;

	bool IsLoaded(Asset asset) {
		return m_AssetRegistry[asset];
	}
	virtual void Load(Asset asset) = 0;
	virtual void Unload(Asset asset) = 0;

	template<typename T>
	Ref<T> Get(Asset asset);

protected:
	Map<Asset, bool> m_AssetRegistry;
	Map<UUID, Ref<Mesh>> m_MeshAssets;
	Map<UUID, Ref<Texture>> m_TextureAssets;
	Map<UUID, Ref<Cubemap>> m_CubemapAssets;
	Map<UUID, Ref<Sound>> m_AudioAssets;

private:
	Ref<Mesh> GetMesh(Asset asset) {
		if(!m_MeshAssets.count(asset.ID))
			return nullptr;
		return m_MeshAssets[asset.ID];
	}
	Ref<Texture> GetTexture(Asset asset) {
		if(!m_TextureAssets.count(asset.ID))
			return nullptr;
		return m_TextureAssets[asset.ID];
	}
	Ref<Cubemap> GetCubemap(Asset asset) {
		if(!m_CubemapAssets.count(asset.ID))
			return nullptr;
		return m_CubemapAssets[asset.ID];
	}
	Ref<Sound> GetSound(Asset asset) {
		if(!m_AudioAssets.count(asset.ID))
			return nullptr;
		return m_AudioAssets[asset.ID];
	}
};

template<> inline
Ref<Mesh> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Mesh);
	return GetMesh(asset);
}

template<> inline
Ref<Texture> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Texture);
	return GetTexture(asset);
}

template<> inline
Ref<Cubemap> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Cubemap);
	return GetCubemap(asset);
}

template<> inline
Ref<Sound> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Audio);
	return GetSound(asset);
}

}
