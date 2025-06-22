#include "AssetManager.h"

#include <bitset>

#include <Magma/Core/BinaryReader.h>

namespace Lava {

RuntimeAssetManager::RuntimeAssetManager() {

}

RuntimeAssetManager::~RuntimeAssetManager() {

}

}

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(Asset& asset) {
	uint64_t id;
	uint8_t type;
	bool primary;
	Read(id);
	Read(type);
	Read(primary);
	asset = { id, (AssetType)type, primary };

	uint64_t refCount;
	Read(refCount);
	for(uint64_t i = 0; i < refCount; i++) {
		Read(id);
		Read(type);
		Asset ref = { id, (AssetType)type, false };
		AssetManager::Get()->AddRef(asset, ref);
	}

	std::string name;
	Read(name);
	if(name != "")
		AssetManager::Get()->NameAsset(asset, name);

	return *this;
}

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
	if(!IsValid(asset) || IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = true;
	BinaryReader pack("./.volc.assetpk");
	pack.SetPosition(m_AssetOffsets[asset.ID]);

	if(asset.Type == AssetType::Mesh) {
		Ref<Mesh> mesh = CreateRef<Mesh>(MeshType::Model);
		pack.Read(mesh->SubMeshes);

		for(const auto& ref : GetRefs(asset)) {
			auto& mat = mesh->Materials.Emplace();
			Load(ref);
			auto material = Get<Magma::Material>(asset);
			// TODO
		}

		m_MeshAssets[asset.ID] = mesh;
	}
	else if(asset.Type == AssetType::Texture) {
		uint32_t width, height;
		pack.Read(width);
		pack.Read(height);
		Buffer<uint8_t> data(width * height * 4);
		pack.ReadData(data.Get(), data.GetMaxCount());

		Ref<Texture> texture = Texture::Create(width, height);
		texture->SetData(data);
		m_TextureAssets[asset.ID] = texture;
	}
	else if(asset.Type == AssetType::Cubemap) {
		uint32_t test;
		pack.Read(test);
		VOLCANICORE_ASSERT(test == 4);

	}
	else if(asset.Type == AssetType::Shader) {
		Buffer<uint32_t> bytecode;
		pack.Read(bytecode);

		// m_ShaderAssets[asset.ID] = ShaderPipeline::Create(bytecode);
	}
	else if(asset.Type == AssetType::Audio) {
		Buffer<float> data;
		pack.Read(data);

		Ref<Sound> sound = CreateRef<Sound>();
		bool success =
			sound->GetInternal()
			.loadRawWave(data.Get(), data.GetCount(), 44100.0f, 1, true, false);
		VOLCANICORE_ASSERT(success == 0);

		m_AudioAssets[asset.ID] = sound;
	}
	else if(asset.Type == AssetType::Script) {
		std::string name;
		pack.Read(name);

		auto handle =
			ScriptEngine::Get()->GetModule(name.c_str(), asGM_ALWAYS_CREATE);
		ByteCodeReader stream(&pack);
		handle->LoadByteCode(&stream);
		m_ScriptAssets[asset.ID] = CreateRef<ScriptModule>(handle);
	}
	else if(asset.Type == AssetType::Material) {
		uint32_t test;
		pack.Read(test);
		VOLCANICORE_ASSERT(test == 5);
		m_MaterialAssets[asset.ID] = CreateRef<Magma::Material>();
		// List<uint8_t> materialFlags;
		// pack.Read(materialFlags);

		// uint64_t refIdx = 0;
		// List<Asset> refs;
		// if(HasRefs(asset))
		// 	refs = m_References[asset.ID];

		// for(uint64_t i = 0; i < materialFlags.Count(); i++) {
		// 	std::bitset<3> flags(materialFlags[i]);
		// 	pack.Read(mesh->Materials[i].DiffuseColor);
		// 	pack.Read(mesh->Materials[i].SpecularColor);
		// 	pack.Read(mesh->Materials[i].EmissiveColor);

		// 	if(flags.test(0)) {
		// 		Load(refs[refIdx]);
		// 		mesh->Materials[i].Diffuse = Get<Texture>(refs[refIdx++]);
		// 	}
		// 	if(flags.test(1)) {
		// 		Load(refs[refIdx]);
		// 		mesh->Materials[i].Specular = Get<Texture>(refs[refIdx++]);
		// 	}
		// 	if(flags.test(2)) {
		// 		Load(refs[refIdx]);
		// 		mesh->Materials[i].Emissive = Get<Texture>(refs[refIdx++]);
		// 	}
		// }
	}
}

void RuntimeAssetManager::Unload(Asset asset) {
	if(!IsValid(asset) || !IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = false;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Shader)
		m_ShaderAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Script)
		m_ScriptAssets.erase(asset.ID);
}

void RuntimeAssetManager::Load() {
	BinaryReader pack("./.volc.assetpk");

	std::string header;
	pack.Read(header);
	VOLCANICORE_ASSERT(header == "VOLC_PACK");

	uint64_t count;
	pack.Read(count);
	for(uint64_t i = 0; i < count; i++) {
		Asset asset;
		pack.Read(asset);

		uint64_t offset;
		pack.Read(offset);
		m_AssetRegistry[asset] = false;
		m_AssetOffsets[asset.ID] = offset;
	}
}

}