#include "AssetManager.h"

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include "AssetImporter.h"

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
		m_MeshAssets[asset.ID] = AssetImporter::GetMesh(path);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = AssetImporter::GetTexture(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = AssetImporter::GetCubemap(path);
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
	m_MeshAssets[newAsset.ID] = Mesh::Create(primitive);

	return newAsset;
}

Asset EditorAssetManager::Add(const std::string& path, AssetType type) {
	Asset newAsset{ UUID(), type };
	m_AssetRegistry[newAsset] = false;
	m_Paths[newAsset.ID] = path;

	return newAsset;
}

UUID EditorAssetManager::GetFromPath(const std::string& path) {
	for(auto [id, assetPath] : m_Paths)
		if(path == assetPath)
			return id;

	return 0;
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
			m_MeshAssets[id] = AssetImporter::GetMesh(m_Paths[id]);
		}
		else {
			m_AssetRegistry[newAsset] = false;
			auto type =
				(MeshPrimitive)meshNode["PrimitiveType"].as<uint32_t>();
			auto materialNode = meshNode["Material"];

			Material mat;
			if(materialNode["Diffuse"])
				mat.Diffuse =
					AssetImporter::GetTexture(
						materialNode["Diffuse"]["Path"].as<std::string>());
			if(materialNode["Specular"])
				mat.Specular =
					AssetImporter::GetTexture(
						materialNode["Specular"]["Path"].as<std::string>());
			if(materialNode["Emissive"])
				mat.Emissive =
					AssetImporter::GetTexture(
						materialNode["Emissive"]["Path"].as<std::string>());

			mat.DiffuseColor = materialNode["DiffuseColor"].as<glm::vec4>();
			mat.SpecularColor = materialNode["SpecularColor"].as<glm::vec4>();
			mat.EmissiveColor = materialNode["EmissiveColor"].as<glm::vec4>();

			m_Primitives[id] = type;
			m_MeshAssets[id] = Mesh::Create(type, mat);
		}
	}
	for(auto textureAssetNode : assetPackNode["TextureAssets"]) {

	}
	for(auto cubemapAssetNode : assetPackNode["CubemapAssets"]) {

	}
	for(auto audioAssetNode : assetPackNode["AudioAssets"]) {

	}
}

void EditorAssetManager::Reload() {
	auto rootPath = fs::path(m_Path).parent_path() / "Asset";
	List<std::string> paths
		{
			(rootPath / "Mesh").string(),
			(rootPath / "Image").string(),
			(rootPath / "Audio").string()
		};

	uint32_t i = 0;
	for(auto folder : paths) {
		for(auto path : FileUtils::GetFiles(folder))
			if(!GetFromPath(path))
				Add(path, (AssetType)i);

		i++;
	}
}

void EditorAssetManager::Save() {
	if(m_Path == "")
		return;

	YAMLSerializer serializer;
	serializer.BeginMapping();

	serializer.WriteKey("AssetPack").BeginMapping();

	serializer.WriteKey("MeshAssets").BeginSequence();
	for(auto& [id, mesh] : m_MeshAssets) {
		serializer.BeginMapping();
		serializer.WriteKey("Mesh").BeginMapping();
		serializer.WriteKey("ID").Write((uint64_t)id);

		std::string path = GetPath(id);
		if(path != "")
			serializer.WriteKey("Path").Write(path);
		else {
			auto subMesh = mesh->SubMeshes[0];
			serializer.WriteKey("PrimitiveType")
				.Write((uint32_t)m_Primitives[id]);

			serializer.WriteKey("Material")
			.BeginMapping();

			auto& mat = subMesh.Mat;
			if(mat.Diffuse)
				serializer.WriteKey("Diffuse")
				.BeginMapping()
					.WriteKey("Path").Write(GetPath(Find(mat.Diffuse).ID))
				.EndMapping();

			if(mat.Specular)
				serializer.WriteKey("Specular")
				.BeginMapping()
					.WriteKey("Path").Write(GetPath(Find(mat.Specular).ID))
				.EndMapping();

			if(mat.Emissive)
				serializer.WriteKey("Emissive")
				.BeginMapping()
					.WriteKey("Path").Write(GetPath(Find(mat.Emissive).ID))
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

	serializer.WriteKey("TextureAssets").BeginSequence();
	for(auto& [id, texture] : m_TextureAssets) {
		// serializer.WriteKey("Path").Write(texture->GetPath());
	}
	serializer.EndSequence();

	serializer.WriteKey("CubemapAssets").BeginSequence();
	for(auto& [id, texture] : m_CubemapAssets) {
		// serializer.WriteKey("Path").Write(texture->GetPath());
	}
	serializer.EndSequence();

	serializer.WriteKey("AudioAssets").BeginSequence();
	// for(auto& [id, texture] : m_AudioAssets) {
	// 	// serializer.WriteKey("Path").Write(texture->GetPath());
	// }
	serializer.EndSequence();

	serializer.EndMapping(); // AssetPack
	serializer.EndMapping();

	serializer.Finalize(m_Path);
}

}

namespace Magma {


}

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