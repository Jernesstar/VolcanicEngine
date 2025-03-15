#include "AssetManager.h"

#include <bitset>

#include <Magma/Core/BinaryReader.h>

#include <VolcaniCore/Core/Math.h>

namespace Lava {

RuntimeAssetManager::RuntimeAssetManager() {

}

RuntimeAssetManager::~RuntimeAssetManager() {

}

}

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(SubMesh& mesh) {
	Read(mesh.Vertices);
	Read(mesh.Indices);
	Read(mesh.MaterialIndex);

	return *this;
}
	
}

namespace Lava {

void RuntimeAssetManager::Load(Asset asset) {
	if(IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = true;
	uint64_t offset = m_AssetOffsets[asset.ID];

	if(asset.Type == AssetType::Mesh) {
		Ref<Mesh> mesh = CreateRef<Mesh>(MeshType::Model);
		BinaryReader reader("Asset/Mesh/mesh.bin");
		m_MeshAssets[asset.ID] = mesh;
		
		VOLCANICORE_LOG_INFO("Here");
		reader.SetPosition(offset);
		reader.Read(mesh->SubMeshes);

		if(!HasRefs(asset)) {
			VOLCANICORE_LOG_INFO("Here2");
			// Engine-generated model and material
			mesh->Materials.Emplace();
			reader.Read(mesh->Materials[0].DiffuseColor);
			reader.Read(mesh->Materials[0].SpecularColor);
			reader.Read(mesh->Materials[0].EmissiveColor);
			return;
		}

		List<Asset>& refs = m_References[asset.ID];
		uint64_t refIdx = 0;
		List<uint8_t> materialFlags;
		reader.Read(materialFlags);

		for(uint64_t i = 0; i < materialFlags.Count(); i++) {
			std::bitset<3> flags(materialFlags[i]);
			mesh->Materials.Emplace();

			if(flags.test(0)) {
				Load(refs[refIdx]);
				mesh->Materials[i].Diffuse = Get<Texture>(refs[refIdx++]);
			}
			if(flags.test(1)) {
				Load(refs[refIdx]);
				mesh->Materials[i].Specular = Get<Texture>(refs[refIdx++]);
			}
			if(flags.test(2)) {
				Load(refs[refIdx]);
				mesh->Materials[i].Emissive = Get<Texture>(refs[refIdx++]);
			}
		}
	}
	else if(asset.Type == AssetType::Texture) {
		BinaryReader reader("Asset/Texture/image.bin");
		reader.SetPosition(offset);

		uint32_t width, height;
		Buffer<uint8_t> data;
		reader.Read(width);
		reader.Read(height);
		reader.Read(data);

		Ref<Texture> texture = Texture::Create(width, height);
		texture->SetData(data);

		m_TextureAssets[asset.ID] = texture;
	}
	else if(asset.Type == AssetType::Cubemap) {

	}
	else if(asset.Type == AssetType::Audio) {

	}
	else if(asset.Type == AssetType::Script) {
		BinaryReader reader("Asset/Script/script.bin");
		std::string name;
		reader.SetPosition(offset);
		reader.Read(name);

		auto path = "Asset/Script/" + name + ".class";
		Ref<ScriptModule> mod = CreateRef<ScriptModule>(name);
		mod->Load(path);

		m_ScriptAssets[asset.ID] = mod;
	}
}

void RuntimeAssetManager::Unload(Asset asset) {
	if(!IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = false;
	uint64_t offset = m_AssetOffsets[asset.ID];

}

void RuntimeAssetManager::Load() {
	BinaryReader reader("./.volc.assetpk");

	std::string header;
	reader.Read(header);
	VOLCANICORE_ASSERT(header == "VOLC_PACK");

	uint64_t count;
	reader.Read(count);
	for(uint64_t i = 0; i < count; i++) {
		uint64_t id;
		uint32_t typeInt;
		reader.Read(id);
		reader.Read(typeInt);
		Asset asset{ id, (AssetType)typeInt };
		uint64_t offset;
		reader.Read(offset);
		m_AssetRegistry[asset] = false;
		m_AssetOffsets[asset.ID] = offset;

		uint64_t refCount;
		reader.Read(refCount);
		if(!refCount)
			continue;

		m_References[asset.ID].Allocate(refCount);
		for(uint64_t i = 0; i < refCount; i++) {
			uint64_t refID;
			uint32_t refType;
			reader.Read(refID);
			reader.Read(refType);
			m_References[asset.ID].Emplace(refID, (AssetType)refType, false);
		}
	}
}

}