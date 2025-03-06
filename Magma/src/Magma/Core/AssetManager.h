#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Mesh.h>

#include <Magma/Audio/Sound.h>

#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>

using namespace VolcaniCore;
using namespace Magma::Audio;
using namespace Magma::Script;

namespace Magma {

enum class AssetType {
	Mesh,
	Texture,
	Cubemap,
	Font,
	Audio,
	Script,
	Shader,
	None
};

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
	Map<UUID, List<Asset>> m_References;

	Map<UUID, Ref<Mesh>> m_MeshAssets;
	Map<UUID, Ref<Texture>> m_TextureAssets;
	Map<UUID, Ref<Cubemap>> m_CubemapAssets;
	Map<UUID, Ref<Sound>> m_AudioAssets;
	Map<UUID, Ref<ScriptModule>> m_ScriptAssets;

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
	Ref<ScriptModule> GetScript(Asset asset) {
		if(!m_ScriptAssets.count(asset.ID))
			return nullptr;
		return m_ScriptAssets[asset.ID];
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

template<> inline
Ref<ScriptModule> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Script);
	return GetScript(asset);
}

}
