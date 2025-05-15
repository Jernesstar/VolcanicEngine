#pragma once

#include <map>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Mesh.h>

#include <Magma/Audio/Sound.h>

#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>

using namespace Magma::Audio;
using namespace Magma::Script;

namespace Magma {

enum class AssetType {
	None,
	Mesh,
	Texture,
	Cubemap,
	Font,
	Audio,
	Script,
	Shader,
};

struct Asset {
	VolcaniCore::UUID ID = 0;
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

class AssetManager : public VolcaniCore::Derivable<AssetManager> {
public:
	static AssetManager* Get() { return s_Instance; }

public:
	AssetManager() {
		s_Instance = this;
	}
	virtual ~AssetManager() = default;

	virtual void Load(Asset asset) = 0;
	virtual void Unload(Asset asset) = 0;

	template<typename T>
	Ref<T> Get(Asset asset);

	void NameAsset(Asset asset, const std::string& name) {
		m_NamedAssets[name] = asset;
	}

	std::string GetAssetName(Asset target) {
		for(auto [name, asset] : m_NamedAssets) {
			if(asset == target)
				return name;
		}

		return "";
	}

	Asset GetNamedAsset(const std::string& name) const {
		if(!m_NamedAssets.count(name))
			return { };
		return m_NamedAssets.at(name);
	}

	void RemoveName(Asset asset) {
		auto name = GetAssetName(asset);
		if(name != "")
			m_NamedAssets.erase(name);
	}

	bool IsLoaded(Asset asset) const {
		return m_AssetRegistry.at(asset);
	}

	bool HasRefs(Asset asset) const {
		return m_References.count(asset.ID);
	}

	bool IsValid(Asset asset) const {
		return asset.ID && asset.Type != AssetType::None
			&& m_AssetRegistry.count(asset);
	}

	void Clear() {
		m_AssetRegistry.clear();
		m_References.clear();

		m_MeshAssets.clear();
		m_TextureAssets.clear();
		m_CubemapAssets.clear();
		m_AudioAssets.clear();
		m_ScriptAssets.clear();
	}

protected:
	std::map<Asset, bool> m_AssetRegistry;
	Map<VolcaniCore::UUID, VolcaniCore::List<Asset>> m_References;
	Map<std::string, Asset> m_NamedAssets;

	Map<VolcaniCore::UUID, Ref<VolcaniCore::Mesh>> m_MeshAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::Texture>> m_TextureAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::Cubemap>> m_CubemapAssets;
	Map<VolcaniCore::UUID, Ref<Sound>> m_AudioAssets;
	Map<VolcaniCore::UUID, Ref<ScriptModule>> m_ScriptAssets;

private:
	inline static AssetManager* s_Instance;

private:
	Ref<VolcaniCore::Mesh> GetMesh(Asset asset) {
		if(!m_MeshAssets.count(asset.ID))
			return nullptr;
		return m_MeshAssets[asset.ID];
	}
	Ref<VolcaniCore::Texture> GetTexture(Asset asset) {
		if(!m_TextureAssets.count(asset.ID))
			return nullptr;
		return m_TextureAssets[asset.ID];
	}
	Ref<VolcaniCore::Cubemap> GetCubemap(Asset asset) {
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
Ref<VolcaniCore::Mesh> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Mesh);
	return GetMesh(asset);
}

template<> inline
Ref<VolcaniCore::Texture> AssetManager::Get(Asset asset) {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Texture);
	return GetTexture(asset);
}

template<> inline
Ref<VolcaniCore::Cubemap> AssetManager::Get(Asset asset) {
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
