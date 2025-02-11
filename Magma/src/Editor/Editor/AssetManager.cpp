#include "AssetManager.h"

#include <VolcaniCore/Core/FileUtils.h>

#include "Core/YAMLSerializer.h"

namespace Magma {

EditorAssetManager::EditorAssetManager() {

}

EditorAssetManager::~EditorAssetManager() {
	Save();
}

void EditorAssetManager::Load(Asset& asset) {
	std::string path = m_Paths[asset.ID];
	m_AssetRegistry[asset.ID] = true;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets[asset.ID] = Model::Create(path);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = Texture::Create(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = Cubemap::Create(path);
}

void EditorAssetManager::Unload(Asset& asset) {
	std::string path = m_Paths[asset.ID];
	m_AssetRegistry[asset.ID] = false;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets.erase(asset.ID);
}

Asset EditorAssetManager::GetFromPath(const std::string& path, AssetType type) {
	for(auto [id, assetPath] : m_Paths)
		if(assetPath == path)
			return Asset{ id, type };

	return Asset{ 0, AssetType::Mesh };
}

void EditorAssetManager::Load(const std::string& path) {
	namespace fs = std::filesystem;
	auto packPath = (fs::path(path) / "Visual" / ".assetpack").string();
	if(!FileUtils::FileExists(packPath))
		return;

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

		if(meshNode["Path"]) {
			m_AssetRegistry[id] = false;
			m_Paths[id] = meshNode["Path"].as<std::string>();
		}
		else {
			m_AssetRegistry[id] = false;
			MeshPrimitive type = (MeshPrimitive)meshNode["Type"].as<uint32_t>();
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
			serializer.WriteKey("Type").Write((uint32_t)m_Primitives[id]);

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

	serializer.WriteKey("TextureAssets").BeginSequence();
	serializer.EndSequence();

	serializer.EndMapping(); // AssetPack
	serializer.EndMapping();
}

void EditorAssetManager::SaveRuntime(const std::string& path) {

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