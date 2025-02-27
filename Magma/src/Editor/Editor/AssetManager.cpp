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
	std::string path = GetPath(asset.ID);
	m_AssetRegistry[asset] = true;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets[asset.ID] = AssetImporter::GetMesh(path);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = AssetImporter::GetTexture(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = AssetImporter::GetCubemap(path);
	else if(asset.Type == AssetType::Cubemap)
		m_AudioAssets[asset.ID] = AssetImporter::GetAudio(path);
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
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets.erase(asset.ID);
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

	if(!assetPackNode) {
		Asset asset{ UUID(), AssetType::Mesh };
		m_AssetRegistry[asset] = true;
		m_MeshAssets[asset.ID] = Mesh::Create(MeshType::Quad);

		Asset asset{ UUID(), AssetType::Mesh };
		m_AssetRegistry[asset] = true;
		m_MeshAssets[asset.ID] = Mesh::Create(MeshType::Cube);
		return;
	}

	for(auto assetNode : assetPackNode["Assets"]) {
		auto node = assetNode["Asset"];
		UUID id = node["ID"].as<uint64_t>();
		AssetType type = (AssetType)node["Type"].as<uint32_t>();
		Asset newAsset = { id, type };

		if(node["Path"]) {
			m_AssetRegistry[newAsset] = false;
			m_Paths[id] = node["Path"].as<std::string>();
		}
		else {
			m_AssetRegistry[newAsset] = false;
			auto type =
				(MeshType)node["Type"].as<uint32_t>();
			auto materialNode = node["Material"];

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

			m_MeshAssets[id] = Mesh::Create(type, mat);
		}
	}
}

void EditorAssetManager::Reload() {
	namespace fs = std::filesystem;

	auto rootPath = fs::path(m_Path).parent_path() / "Asset";
	List<std::string> paths
		{
			(rootPath / "Mesh").string(),
			(rootPath / "Image").string(),
			(rootPath / "Cubemap").string(),
			(rootPath / "Font").string(),
			(rootPath / "Audio").string(),
			(rootPath / "Shader").string()
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

	serializer.WriteKey("Assets").BeginSequence();
	for(auto& [asset, _] : m_AssetRegistry) {
		serializer.BeginMapping();
		serializer.WriteKey("Asset").BeginMapping();
		serializer.WriteKey("ID").Write((uint64_t)asset.ID);
		serializer.WriteKey("Type").Write((uint32_t)asset.Type);

		std::string path = GetPath(asset.ID);
		if(path != "")
			serializer.WriteKey("Path").Write(path);
		else {
			auto mesh = m_MeshAssets[asset.ID];
			auto subMesh = mesh->SubMeshes[0];
			serializer.WriteKey("PrimitiveType")
				.Write((uint32_t)m_Primitives[asset.ID]);

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

		serializer.EndMapping(); // Asset
		serializer.EndMapping();
	}
	serializer.EndSequence();

	serializer.EndMapping(); // AssetPack
	serializer.EndMapping();

	serializer.Finalize(m_Path);
}

}

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const SubMesh& mesh) {
	WriteData(mesh.Vertices.GetBuffer());
	WriteData(mesh.Indices.GetBuffer());
	Write(mesh.MaterialIndex);

	return *this;
}

}

namespace Magma {

void EditorAssetManager::RuntimeSave(const std::string& path) {
	namespace fs = std::filesystem;

	BinaryWriter pack((fs::path(path) / ".volc.assetpk").string());
	BinaryWriter meshFile((fs::path(path) / "Asset" / "Mesh").string());
	BinaryWriter textureFile((fs::path(path) / "Asset" / "Texture").string());
	BinaryWriter soundFile((fs::path(path) / "Asset" / "Sound").string());

	for(auto [asset, _] : m_AssetRegistry) {
		pack.Write((uint64_t)asset.ID);

		if(asset.Type == AssetType::Mesh) {
			auto mesh = Get<Mesh>(asset);
			auto ref = m_References[asset.ID];

			pack.Write(meshFile.GetPosition());
			meshFile.Write(mesh->SubMeshes);
		}
		else if(asset.Type == AssetType::Texture) {
			ImageData image = AssetImporter::GetImageData(m_Paths[asset.ID]);

			pack.Write(textureFile.GetPosition());
			textureFile.Write(image.Width);
			textureFile.Write(image.Height);
			textureFile.Write(image.Data);
		}
		else if(asset.Type == AssetType::Sound) {
			auto sound = Get<Sound>(asset);
			pack.Write(soundFile.GetPosition());
			soundFile.Write(sound->GetData());
		}
	}
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