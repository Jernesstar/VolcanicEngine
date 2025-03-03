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
BinaryReader& BinaryReader::ReadObject(SubMesh& mesh) {
	Read(mesh.Vertices);
	Read(mesh.Indices);
	Read(mesh.MaterialIndex);

	return *this;
}
	
}

namespace Lava {

void RuntimeAssetManager::Load(Asset asset) {
	uint64_t offset = m_AssetOffsets[asset.ID];

	if(asset.Type == AssetType::Mesh) {
		Ref<Mesh> mesh = CreateRef<Mesh>(MeshType::Model);
		BinaryReader reader("Asset/Mesh/mesh.bin");
		reader.SetPosition(offset);
		reader.Read(mesh->SubMeshes);
		List<uint64_t> materialFlags;
		reader.Read(materialFlags);

		auto& refs = m_References[asset.ID];
		uint64_t i = 0;
		for(auto mat : materialFlags) {
			std::bitset<3> flags(mat);
			if(flags.test(2))
				Load(refs[i++]);
			if(flags.test(1))
				Load(refs[i++]);
			if(flags.test(0))
				Load(refs[i++]);
		}

		m_MeshAssets[asset.ID] = mesh;
	}
	else if(asset.Type == AssetType::Texture) {
		BinaryReader reader("Asset/Image/image.bin");
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
}

void RuntimeAssetManager::Unload(Asset asset) {

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
		m_References[asset.ID].Allocate(refCount);
		for(uint64_t i = 0; i < refCount; i++) {
			uint64_t refID;
			uint32_t refType;
			reader.Read(refID);
			reader.Read(refType);
			Asset asset{ refID, (AssetType)refType };
			m_References[asset.ID].Add(asset);
		}
	}
}

}