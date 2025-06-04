#pragma once

#include <map>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Shader.h>

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
	Shader,
	Font,
	Audio,
	Script,
	Material,
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
	Ref<T> Get(Asset asset) const;
	
	bool IsValid(Asset asset) const {
		return asset.ID && asset.Type != AssetType::None
			&& m_AssetRegistry.count(asset);
	}

	bool IsLoaded(Asset asset) const { return m_AssetRegistry.at(asset); }

	bool HasRefs(Asset asset) const { return m_References.count(asset.ID); }

	VolcaniCore::List<Asset> GetRefs(Asset asset) const {
		return m_References.at(asset.ID);
	}

	void NameAsset(Asset asset, const std::string& name) {
		m_NamedAssets[asset] = name;
	}

	void RemoveName(Asset asset) { m_NamedAssets.erase(asset); }

	std::string GetAssetName(Asset asset) const {
		if(!m_NamedAssets.count(asset))
			return "";
		return m_NamedAssets.at(asset);
	}

	Asset GetNamedAsset(const std::string& lookup) const {
		for(auto [asset, name] : m_NamedAssets) {
			if(name == lookup)
				return asset;
		}

		return { };
	}

	void Clear() {
		m_AssetRegistry.clear();
		m_References.clear();

		m_MeshAssets.clear();
		m_TextureAssets.clear();
		m_CubemapAssets.clear();
		m_ShaderAssets.clear();
		m_AudioAssets.clear();
		m_ScriptAssets.clear();
	}

protected:
	std::map<Asset, bool> m_AssetRegistry;
	Map<VolcaniCore::UUID, VolcaniCore::List<Asset>> m_References;
	Map<Asset, std::string> m_NamedAssets;

	Map<VolcaniCore::UUID, Ref<VolcaniCore::Mesh>> m_MeshAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::Texture>> m_TextureAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::Cubemap>> m_CubemapAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::ShaderPipeline>> m_ShaderAssets;
	Map<VolcaniCore::UUID, Ref<Sound>> m_AudioAssets;
	Map<VolcaniCore::UUID, Ref<ScriptModule>> m_ScriptAssets;
	Map<VolcaniCore::UUID, Ref<VolcaniCore::DrawUniforms>> m_MaterialAssets;

private:
	inline static AssetManager* s_Instance;
};

template<>
inline Ref<VolcaniCore::Mesh> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Mesh);
	if(!m_MeshAssets.count(asset.ID))
		return nullptr;
	return m_MeshAssets.at(asset.ID);
}

template<>
inline Ref<VolcaniCore::Texture> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Texture);
	if(!m_TextureAssets.count(asset.ID))
		return nullptr;
	return m_TextureAssets.at(asset.ID);
}

template<>
inline Ref<VolcaniCore::Cubemap> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Cubemap);
	if(!m_CubemapAssets.count(asset.ID))
		return nullptr;
	return m_CubemapAssets.at(asset.ID);
}

template<>
inline Ref<VolcaniCore::ShaderPipeline> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Shader);
	if(!m_ShaderAssets.count(asset.ID))
		return nullptr;
	return m_ShaderAssets.at(asset.ID);
}

template<>
inline Ref<Sound> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Audio);
	if(!m_AudioAssets.count(asset.ID))
		return nullptr;
	return m_AudioAssets.at(asset.ID);
}

template<>
inline Ref<ScriptModule> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Script);
	if(!m_ScriptAssets.count(asset.ID))
		return nullptr;
	return m_ScriptAssets.at(asset.ID);
}

template<>
inline Ref<VolcaniCore::DrawUniforms> AssetManager::Get(Asset asset) const {
	VOLCANICORE_ASSERT(asset.Type == AssetType::Material);
	if(!m_MaterialAssets.count(asset.ID))
		return nullptr;
	return m_MaterialAssets.at(asset.ID);
}

}
