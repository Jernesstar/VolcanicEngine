#include "AssetManager.h"

#include <bitset>

#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include "AssetImporter.h"

namespace fs = std::filesystem;

namespace Magma {

EditorAssetManager::EditorAssetManager() {

}

EditorAssetManager::~EditorAssetManager() {
	Save();
}

void EditorAssetManager::Load(Asset asset) {
	if(IsLoaded(asset))
		return;

	std::string path = GetPath(asset.ID);
	m_AssetRegistry[asset] = true;
	if(path == "")
		return;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets[asset.ID] = AssetImporter::GetMesh(path);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = AssetImporter::GetTexture(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = AssetImporter::GetCubemap(path);
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets[asset.ID] = AssetImporter::GetAudio(path);
	else if(asset.Type == AssetType::Script) {
		auto name = fs::path(path).filename().stem().string();
		Ref<ScriptModule> mod = CreateRef<ScriptModule>(name);
		mod->Load(path);
		m_ScriptAssets[asset.ID] = mod;
	}
}

void EditorAssetManager::Unload(Asset asset) {
	if(!IsLoaded(asset))
		return;

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
	for(auto& [id, assetPath] : m_Paths)
		if(fs::path(path) == fs::path(assetPath))
			return id;

	return 0;
}

std::string EditorAssetManager::GetPath(UUID id) {
	if(!m_Paths.count(id))
		return "";

	return m_Paths[id];
}

void EditorAssetManager::Load(const std::string& path) {
	auto packPath =
		(fs::path(path).parent_path() / "Visual" / ".magma.assetpk").string();
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

	auto rootPath = fs::path(path).parent_path() / "Visual" / "Asset";
	for(auto assetNode : assetPackNode["Assets"]) {
		auto node = assetNode["Asset"];
		UUID id = node["ID"].as<uint64_t>();
		AssetType type = (AssetType)node["Type"].as<uint32_t>();
		Asset asset = { id, type };
		m_AssetRegistry[asset] = false;

		if(node["Path"])
			m_Paths[id] = (rootPath / node["Path"].as<std::string>()).string();
		if(asset.Type == AssetType::Mesh) {
			if(node["Path"]) {
				auto materials = AssetImporter::GetMeshMaterials(m_Paths[id]);
				for(auto matPaths : materials) {
					for(uint32_t i = 0; i < 3; i++) {
						auto path = matPaths[i];
						if(path == "")
							continue;
						auto& secondary =
							m_References[asset.ID].Emplace(
								UUID(), AssetType::Texture, false);
						m_AssetRegistry[secondary] = false;
						m_Paths[secondary.ID] = path;
					}
				}

				continue;
			}

			auto type = (MeshType)node["MeshType"].as<uint32_t>();
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
	List<fs::path> paths
	{
		(rootPath / "Mesh"),
		(rootPath / "Image"),
		(rootPath / "Cubemap"),
		(rootPath / "Font"),
		(rootPath / "Audio"),
		(rootPath / "Script"),
		(rootPath / "Shader")
	};

	uint32_t i = 0;
	for(auto& folder : paths) {
		for(auto p : FileUtils::GetFiles(folder.string())) {
			std::string path = p;
			if(i == 0)
				path = FileUtils::GetFiles(p, { ".obj" })[0];
			if(!GetFromPath(path))
				Add(path, (AssetType)i);
		}
		i++;
	}
}

void EditorAssetManager::Save() {
	namespace fs = std::filesystem;

	if(m_Path == "")
		return;
	auto rootPath = fs::path(m_Path).parent_path() / "Asset";

	YAMLSerializer serializer;
	serializer.BeginMapping();

	serializer.WriteKey("AssetPack").BeginMapping();

	serializer.WriteKey("Assets").BeginSequence();
	for(auto& [asset, _] : m_AssetRegistry) {
		if(!asset.Primary)
			continue;

		serializer.BeginMapping();
		serializer.WriteKey("Asset").BeginMapping();
		serializer.WriteKey("ID").Write((uint64_t)asset.ID);
		serializer.WriteKey("Type").Write((uint32_t)asset.Type);

		std::string path = GetPath(asset.ID);
		if(path != "")
			serializer.WriteKey("Path")
				.Write(fs::relative(path, rootPath).generic_string());
		else {
			List<Asset>& refs = m_References[asset.ID];
			Ref<Mesh> mesh = m_MeshAssets[asset.ID];
			const SubMesh& subMesh = mesh->SubMeshes[0];
			serializer.WriteKey("MeshType").Write((uint32_t)mesh->Type);

			serializer.WriteKey("Material")
			.BeginMapping();

			auto& mat = mesh->Materials[subMesh.MaterialIndex];
			if(mat.Diffuse)
				serializer.WriteKey("Diffuse")
				.BeginMapping()
					.WriteKey("Path").Write(m_Paths[refs[0].ID])
				.EndMapping();

			if(mat.Specular)
				serializer.WriteKey("Specular")
				.BeginMapping()
					.WriteKey("Path").Write(m_Paths[refs[1].ID])
				.EndMapping();

			if(mat.Emissive)
				serializer.WriteKey("Emissive")
				.BeginMapping()
					.WriteKey("Path").Write(m_Paths[refs[2].ID])
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
	Write(mesh.Vertices);
	Write(mesh.Indices);
	Write(mesh.MaterialIndex);

	return *this;
}

}

namespace Magma {

void EditorAssetManager::RuntimeSave(const std::string& exportPath) {
	namespace fs = std::filesystem;

	auto assetPath = fs::path(exportPath) / "Asset";
	fs::create_directories(assetPath);
	fs::create_directories(assetPath / "Mesh");
	fs::create_directories(assetPath / "Texture");
	fs::create_directories(assetPath / "Audio");
	fs::create_directories(assetPath / "Script");
	fs::create_directories(assetPath / "Shader");

	BinaryWriter pack((fs::path(exportPath) / ".volc.assetpk").string());
	BinaryWriter meshFile((assetPath / "Mesh" / "mesh.bin").string());
	BinaryWriter textureFile((assetPath / "Texture" / "image.bin").string());
	BinaryWriter soundFile((assetPath / "Audio" / "sound.bin").string());
	BinaryWriter scriptFile((assetPath / "Script" / "script.bin").string());

	pack.Write(std::string("VOLC_PACK"));
	pack.Write(m_AssetRegistry.size());

	for(auto [asset, _] : m_AssetRegistry) {
		pack.Write((uint64_t)asset.ID);
		pack.Write((uint32_t)asset.Type);

		auto path = m_Paths[asset.ID];
		if(asset.Type == AssetType::Mesh) {
			pack.Write(meshFile.GetPosition());

			if(path != "") {
				List<MaterialPaths> materials;
				List<SubMesh> meshes;
				AssetImporter::GetMeshData(path, meshes, materials);
				meshFile.Write(meshes);
				meshFile.Write(materials.Count());
				for(auto& mat : materials) {
					std::bitset<3> flags;
					flags |= (mat[0] != "") << 0; // Diffuse
					flags |= (mat[1] != "") << 1; // Specular
					flags |= (mat[2] != "") << 2; // Emissive
					meshFile.Write((uint8_t)flags.to_ulong());
				}
			}
			else {
				// mesh = Get<Mesh>(asset);
				// meshFile.Write(mesh->SubMeshes);
			}
		}
		else if(asset.Type == AssetType::Texture) {
			pack.Write(textureFile.GetPosition());

			ImageData image = AssetImporter::GetImageData(path);
			textureFile.Write(image.Width);
			textureFile.Write(image.Height);
			textureFile.Write(image.Data);
		}
		else if(asset.Type == AssetType::Cubemap) {
			// pack.Write(textureFile.GetPosition());
		}
		else if(asset.Type == AssetType::Audio) {
			pack.Write(soundFile.GetPosition());

			Buffer<float> soundData = AssetImporter::GetAudioData(path);
			soundFile.Write(soundData);
		}
		else if(asset.Type == AssetType::Script) {
			pack.Write(scriptFile.GetPosition());

			Load(asset);
			auto mod = Get<ScriptModule>(asset);
			auto outputPath = assetPath / "Script" / mod->Name;
			mod->Save(outputPath.string() + ".class");
			scriptFile.Write(mod->Name);
		}
		else if(asset.Type == AssetType::Shader) {

		}

		if(m_References.count(asset.ID)) {
			auto& refs = m_References[asset.ID];
			pack.Write(refs.Count());
			for(auto& ref : refs) {
				pack.Write((uint64_t)ref.ID);
				pack.Write((uint32_t)ref.Type);
			}
		}
		else
			pack.Write((uint64_t)0);
	}

	Application::PushDir();

	for(auto name : { "Framebuffer", "Lighting", "Bloom" }) {
		auto sourceRoot = fs::path("Magma") / "assets" / "shaders" / name;
		auto source1 = sourceRoot.string() + ".glsl.vert";
		auto source2 = sourceRoot.string() + ".glsl.frag";

		auto targetRoot = fs::path(exportPath) / "Asset" / "Shader" / name;
		auto target1 = targetRoot.string() + ".glsl.vert";
		auto target2 = targetRoot.string() + ".glsl.frag";

		if(FileUtils::FileExists(target1))
			fs::remove(target1);
		if(FileUtils::FileExists(source1))
			fs::copy_file(source1, target1);
		if(FileUtils::FileExists(target2))
			fs::remove(target2);
		if(FileUtils::FileExists(source2))
			fs::copy_file(source2, target2);
	}

	Application::PopDir();
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