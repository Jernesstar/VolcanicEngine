#include "AssetManager.h"

#include <bitset>

#include <VolcaniCore/Core/Math.h>

#include <Magma/Core/BinaryReader.h>

namespace Lava {

RuntimeAssetManager::RuntimeAssetManager() {

	// Magma assets
	m_AssetRegistry[Asset{ 10012345, AssetType::Mesh }] = true;
	m_MeshAssets[10012345] = Mesh::Create(MeshType::Cube);
	m_AssetRegistry[Asset{ 10112345, AssetType::Mesh }] = true;
	m_MeshAssets[10112345] = Mesh::Create(MeshType::Quad);
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

template<>
BinaryReader& BinaryReader::ReadObject(Mat2& mat) {
	ReadData(glm::value_ptr(mat), sizeof(Mat2));
	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(Mat3& mat) {
	ReadData(glm::value_ptr(mat), sizeof(Mat3));
	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(Mat4& mat) {
	ReadData(glm::value_ptr(mat), sizeof(Vec4));
	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UUID& uuid) {
	uint64_t id;
	Read(id);
	uuid = id;
	return *this;
}

}

namespace Lava {

void RuntimeAssetManager::Load(Asset asset) {
	if(!IsValid(asset) || IsLoaded(asset) || IsNativeAsset(asset))
		return;

	m_AssetRegistry[asset] = true;
	BinaryReader pack("./.volc.assetpk");
	pack.SetPosition(m_AssetOffsets[asset.ID]);

	if(asset.Type == AssetType::Mesh) {
		Ref<Mesh> mesh = CreateRef<Mesh>(MeshType::Model);
		pack.Read(mesh->SubMeshes);
		m_MeshAssets[asset.ID] = mesh;

		for(auto& materialRef : GetRefs(asset)) {
			auto& mat = mesh->Materials.Emplace();
			Load(materialRef);
			auto material = Get<Magma::Material>(materialRef);

			if(material->TextureUniforms.count("u_Diffuse")) {
				auto id = material->TextureUniforms["u_Diffuse"];
				Asset textureAsset = { id, AssetType::Texture };
				Load(textureAsset);
				mat.Diffuse = Get<Texture>(textureAsset);
			}

			if(material->TextureUniforms.count("u_Specular")) {
				auto id = material->TextureUniforms["u_Specular"];
				Asset textureAsset = { id, AssetType::Texture };
				Load(textureAsset);
				mat.Specular = Get<Texture>(textureAsset);
			}

			if(material->TextureUniforms.count("u_Emissive")) {
				auto id = material->TextureUniforms.count("u_Emissive");
				Asset textureAsset = { id, AssetType::Texture };
				Load(textureAsset);
				mat.Emissive = Get<Texture>(textureAsset);
			}

			if(material->TextureUniforms.count("u_DiffuseColor"))
				mat.DiffuseColor = material->Vec4Uniforms["u_DiffuseColor"];
			if(material->TextureUniforms.count("u_SpecularColor"))
				mat.SpecularColor = material->Vec4Uniforms["u_SpecularColor"];
			if(material->TextureUniforms.count("u_EmissiveColor"))
				mat.EmissiveColor = material->Vec4Uniforms["u_EmissiveColor"];
		}
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
		auto mat = CreateRef<Magma::Material>();
		pack.Read(mat->IntUniforms);
		pack.Read(mat->FloatUniforms);
		pack.Read(mat->Vec2Uniforms);
		pack.Read(mat->Vec3Uniforms);
		pack.Read(mat->Vec4Uniforms);
		pack.Read(mat->Mat2Uniforms);
		pack.Read(mat->Mat3Uniforms);
		pack.Read(mat->Mat4Uniforms);

		m_MaterialAssets[asset.ID] = mat;
	}
}

void RuntimeAssetManager::Unload(Asset asset) {
	if(!IsValid(asset) || !IsLoaded(asset) || IsNativeAsset(asset))
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