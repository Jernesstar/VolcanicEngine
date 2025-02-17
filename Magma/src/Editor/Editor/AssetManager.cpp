#include "AssetManager.h"

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

namespace Magma {

EditorAssetManager::EditorAssetManager() {

}

EditorAssetManager::~EditorAssetManager() {
	Save();
}

void EditorAssetManager::Load(Asset asset) {
	std::string path = m_Paths[asset.ID];
	m_AssetRegistry[asset] = true;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets[asset.ID] = Model::Create(path);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = Texture::Create(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = Cubemap::Create(path);
}

void EditorAssetManager::Unload(Asset asset) {
	std::string path = m_Paths[asset.ID];
	m_AssetRegistry[asset] = false;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets.erase(asset.ID);
}

Asset EditorAssetManager::Add(MeshPrimitive primitive) {
	Asset newAsset{ UUID(), AssetType::Mesh };
	m_AssetRegistry[newAsset] = true;

	m_Primitives[newAsset.ID] = primitive;
	m_MeshAssets[newAsset.ID] = Model::Create("");
	m_MeshAssets[newAsset.ID]->AddMesh(Mesh::Create(primitive));

	return newAsset;
}

Asset EditorAssetManager::Add(const std::string& path, AssetType type) {
	Asset newAsset{ UUID(), type };
	m_AssetRegistry[newAsset] = true;

	m_Paths[newAsset.ID] = path;
	m_MeshAssets[newAsset.ID] = Model::Create(path);

	if(type == AssetType::Mesh)
		m_MeshAssets[newAsset.ID] = Model::Create(path);
	else if(type == AssetType::Texture)
		m_TextureAssets[newAsset.ID] = Texture::Create(path);
	else if(type == AssetType::Cubemap)
		m_CubemapAssets[newAsset.ID] = Cubemap::Create(path);

	return newAsset;
}

std::string EditorAssetManager::GetPath(UUID id) {
	if(!m_Paths.count(id))
		return "";
	
	return m_Paths[id];
}

void EditorAssetManager::Load(const std::string& path) {
	namespace fs = std::filesystem;

	auto packPath = (fs::path(path) / "Visual" / ".magma.assetpk").string();
	m_Path = packPath;

	YAML::Node file;
	try {
		file = YAML::LoadFile(m_Path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								m_Path.c_str(), e.what());
	}
	auto assetPackNode = file["AssetPack"];

	if(!assetPackNode)
		return;

	for(auto meshAssetNode : assetPackNode["MeshAssets"]) {
		auto meshNode = meshAssetNode["Mesh"];
		UUID id = meshNode["ID"].as<uint64_t>();
		Asset newAsset = { id, AssetType::Mesh };

		if(meshNode["Path"]) {
			m_AssetRegistry[newAsset] = false;
			m_Paths[id] = meshNode["Path"].as<std::string>();
			m_MeshAssets[id] = Model::Create(m_Paths[id]);
		}
		else {
			m_AssetRegistry[newAsset] = false;
			auto type =
				(MeshPrimitive)meshNode["PrimitiveType"].as<uint32_t>();
			auto materialNode = meshNode["Material"];

			Material mat;
			if(materialNode["Diffuse"])
				mat.Diffuse =
					Texture::Create(
						materialNode["Diffuse"]["Path"].as<std::string>());
			if(materialNode["Specular"])
				mat.Specular =
					Texture::Create(
						materialNode["Specular"]["Path"].as<std::string>());
			if(materialNode["Emissive"])
				mat.Emissive =
					Texture::Create(
						materialNode["Emissive"]["Path"].as<std::string>());

			mat.DiffuseColor = materialNode["DiffuseColor"].as<glm::vec4>();
			mat.SpecularColor = materialNode["SpecularColor"].as<glm::vec4>();
			mat.EmissiveColor = materialNode["EmissiveColor"].as<glm::vec4>();

			m_Primitives[id] = type;
			m_MeshAssets[id] = Model::Create("");
			m_MeshAssets[id]->AddMesh(Mesh::Create(type, mat));
		}
	}
}

void EditorAssetManager::Save() {
	if(m_Path == "")
		return;

	YAMLSerializer serializer;
	serializer.BeginMapping();

	serializer.WriteKey("AssetPack").BeginMapping();

	serializer.WriteKey("MeshAssets").BeginSequence();
	for(auto& [id, model] : m_MeshAssets) {
		serializer.BeginMapping();
		serializer.WriteKey("Mesh").BeginMapping();
		serializer.WriteKey("ID").Write((uint64_t)id);

		if(model->Path != "")
			serializer.WriteKey("Path").Write(model->Path);
		else {
			auto mesh = model->GetMesh(0);
			serializer.WriteKey("PrimitiveType")
				.Write((uint32_t)m_Primitives[id]);

			serializer.WriteKey("Material")
			.BeginMapping();

			auto& mat = mesh->GetMaterial();
			if(mat.Diffuse)
				serializer.WriteKey("Diffuse")
				.BeginMapping()
					.WriteKey("Path").Write(mat.Diffuse->GetPath())
				.EndMapping();

			if(mat.Specular)
				serializer.WriteKey("Specular")
				.BeginMapping()
					.WriteKey("Path").Write(mat.Specular->GetPath())
				.EndMapping();

			if(mat.Emissive)
				serializer.WriteKey("Emissive")
				.BeginMapping()
					.WriteKey("Path").Write(mat.Emissive->GetPath())
				.EndMapping();

			serializer
				.WriteKey("DiffuseColor").Write(mat.DiffuseColor);
			serializer
				.WriteKey("SpecularColor").Write(mat.SpecularColor);
			serializer
				.WriteKey("EmissiveColor").Write(mat.EmissiveColor);

			serializer
			.EndMapping(); // Material
		}

		serializer.EndMapping(); // Mesh
		serializer.EndMapping();
	}
	serializer.EndSequence();

	// serializer.WriteKey("TextureAssets").BeginSequence();
	// serializer.EndSequence();

	serializer.EndMapping(); // AssetPack
	serializer.EndMapping();

	serializer.Finalize(m_Path);
}

}

namespace Magma {


}

// #define 

namespace Magma {

void EditorAssetManager::RuntimeSave(const std::string& path) {
	namespace fs = std::filesystem;

	auto dataPath = (fs::path(path) / ".volc.assetpk").string();
	BinaryWriter writer(dataPath);

}

}

namespace YAML {

template<>
struct convert<glm::vec4> {
	static Node encode(const glm::vec4& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.push_back(v.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec4& v) {
		if(!node.IsSequence() || node.size() != 4)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		v.w = node[3].as<float>();
		return true;
	}
};

}