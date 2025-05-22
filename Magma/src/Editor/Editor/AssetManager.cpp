#include "AssetManager.h"

#include <iostream>
#include <bitset>

#include <efsw/efsw.hpp>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>
#include <VolcaniCore/Core/Math.h>

#include <Magma/Core/YAMLSerializer.h>
#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include <Lava/Core/App.h>

#include "AssetImporter.h"

namespace fs = std::filesystem;

namespace Magma {

std::string AssetTypeToString(AssetType type) {
	switch(type) {
		case AssetType::Mesh:
			return "Mesh";
		case AssetType::Texture:
			return "Texture";
		case AssetType::Cubemap:
			return "Cubemap";
		case AssetType::Font:
			return "Font";
		case AssetType::Audio:
			return "Audio";
		case AssetType::Script:
			return "Script";
		case AssetType::Shader:
			return "Shader";
	}

	return "None";
}

AssetType AssetTypeFromString(const std::string& str) {
	if(str == "Mesh")
		return AssetType::Mesh;
	else if(str == "Texture")
		return AssetType::Texture;
	else if(str == "Cubemap")
		return AssetType::Cubemap;
	else if(str == "Font")
		return AssetType::Font;
	else if(str == "Audio")
		return AssetType::Audio;
	else if(str == "Script")
		return AssetType::Script;
	else if(str == "Shader")
		return AssetType::Shader;

	return AssetType::None;
}

class FileWatcher : public efsw::FileWatchListener {
public:
	FileWatcher(EditorAssetManager* assetManager);
	~FileWatcher() = default;

	void handleFileAction(efsw::WatchID id, const std::string& dir,
		const std::string& file, efsw::Action action, std::string old) override;

	void Add(AssetType type, const std::string& path);
	void Remove(AssetType type, const std::string& path);

	void ReloadMesh(const std::string& path);
	void ReloadTexture(const std::string& path);
	void ReloadAudio(const std::string& path);
	void ReloadScript(const std::string& path);

	void AddCallback(const Func<void, Asset, bool>& callback);

private:
	EditorAssetManager* m_AssetManager;
	List<Func<void, Asset, bool>> m_Callbacks;
};

static bool s_Flip = false;

FileWatcher::FileWatcher(EditorAssetManager* assetManager)
	: m_AssetManager(assetManager) { }

void FileWatcher::handleFileAction(
	efsw::WatchID id, const std::string& dir,
	const std::string& file, efsw::Action action, std::string oldFilename)
{
	auto fullPath = (fs::path(dir) / file).string();
	auto assetDir = fs::path(dir).parent_path().filename().string();
	AssetType type = AssetTypeFromString(assetDir);
	Asset asset = { m_AssetManager->GetFromPath(fullPath), type };

	if(action == efsw::Actions::Add)
		Add(type, fullPath);
	else if(action == efsw::Actions::Delete)
		Remove(type, fullPath);
	else if(action == efsw::Actions::Modified) {
		// Two modified events are emitted for each change
		// So ignore the first and use only the second
		if(!s_Flip) {
			s_Flip = true;
			return;
		}
		else {
			s_Flip = false;
			for(auto callback : m_Callbacks)
				callback(asset, 0);

			switch(type) {
				case AssetType::Mesh:
					ReloadMesh(fullPath);
					break;
				case AssetType::Texture:
					ReloadTexture(fullPath);
					break;
				case AssetType::Audio:
					ReloadAudio(fullPath);
					break;
				case AssetType::Script:
					ReloadScript(fullPath);
					break;
			}

			for(auto callback : m_Callbacks)
				callback(asset, 1);
		}
	}
	else if(action == efsw::Actions::Moved) {
		std::cout << "DIR (" << dir << ") FILE (" << file << ") has event Moved from (" << oldFilename << ")" << "\n";
	}
	else {
		std::cout << "Should never happen!" << "\n";
	}
}

void FileWatcher::Add(AssetType type, const std::string& path) {
	VOLCANICORE_LOG_INFO("New AssetType::%s added at path '%s'",
		AssetTypeToString(type).c_str(), path.c_str());
	m_AssetManager->Add(path, type);
}

void FileWatcher::Remove(AssetType type, const std::string& path) {

}

void FileWatcher::AddCallback(const Func<void, Asset, bool>& callback) {
	m_Callbacks.Add(callback);
}

void FileWatcher::ReloadMesh(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Mesh at path '%s'", path.c_str());
	auto id = m_AssetManager->GetFromPath(path);
	m_AssetManager->m_MeshAssets[id] = AssetImporter::GetMesh(path);
}

void FileWatcher::ReloadTexture(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Texture at path '%s'", path.c_str());
	auto id = m_AssetManager->GetFromPath(path);
	m_AssetManager->m_TextureAssets[id] = AssetImporter::GetTexture(path);
}

void FileWatcher::ReloadAudio(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Audio at path '%s'", path.c_str());
	auto id = m_AssetManager->GetFromPath(path);
	m_AssetManager->m_AudioAssets[id] = AssetImporter::GetAudio(path);
}

void FileWatcher::ReloadScript(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Script at path '%s'", path.c_str());
	auto id = m_AssetManager->GetFromPath(path);
	auto name = fs::path(path).filename().stem().string();
	Ref<ScriptModule> mod = CreateRef<ScriptModule>(name);
	mod->Load(path);

	if(!mod->HasErrors())
		m_AssetManager->m_ScriptAssets[id] = mod;
	else
		VOLCANICORE_LOG_INFO("Error occured when reloading script");
}

static efsw::FileWatcher* s_FileWatcher;
static FileWatcher* s_Listener;
static List<efsw::WatchID> s_WatcherIDs;

EditorAssetManager::EditorAssetManager() {
	s_FileWatcher = new efsw::FileWatcher();
	s_Listener = new FileWatcher(this);
}

EditorAssetManager::~EditorAssetManager() {
	s_WatcherIDs.ForEach([](auto& id) { s_FileWatcher->removeWatch(id); });
	s_WatcherIDs.Clear();
	delete s_Listener;
	delete s_FileWatcher;
}

void EditorAssetManager::Load(Asset asset) {
	if(!IsValid(asset) || IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = true;
	std::string path = GetPath(asset.ID);
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
	if(!IsValid(asset) || !IsLoaded(asset))
		return;

	m_AssetRegistry[asset] = false;

	if(asset.Type == AssetType::Mesh)
		m_MeshAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Script)
		m_ScriptAssets.erase(asset.ID);
}

void EditorAssetManager::AddReloadCallback(
		const Func<void, Asset, bool>& callback)
{
	s_Listener->AddCallback(callback);
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
	auto rootPath = fs::path(path) / "Asset";
	m_Path = (rootPath / ".magma.assetpk").string();

	YAML::Node file;
	try {
		file = YAML::LoadFile(m_Path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								m_Path.c_str(), e.what());
	}
	auto meshPath = rootPath / "Mesh";
	auto scriptPath = rootPath / "Script";

	s_WatcherIDs.Add(
		s_FileWatcher->addWatch(meshPath.string(), s_Listener));
	s_WatcherIDs.Add(
		s_FileWatcher->addWatch(scriptPath.string(), s_Listener));

	s_FileWatcher->watch();

	auto assetPackNode = file["AssetPack"];
	if(!assetPackNode)
		return;

	for(auto assetNode : assetPackNode["Assets"]) {
		auto node = assetNode["Asset"];
		UUID id = node["ID"].as<uint64_t>();
		AssetType type = AssetTypeFromString(node["Type"].as<std::string>());
		Asset asset = { id, type };
		m_AssetRegistry[asset] = false;
		if(node["Name"])
			NameAsset(asset, node["Name"].as<std::string>());

		if(node["Path"])
			m_Paths[id] =
				(rootPath / node["Path"].as<std::string>()).generic_string();
		if(asset.Type == AssetType::Mesh) {
			if(node["Path"]) {
				List<SubMesh> meshes;
				List<MaterialPaths> materials;
				AssetImporter::GetMeshData(m_Paths[id], meshes, materials);
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
		for(auto path : FileUtils::GetFiles(folder.string())) {
			if(i == 0)
				path = FileUtils::GetFiles(path, { ".obj" })[0];
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
	auto rootPath = fs::path(m_Path).parent_path();

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
		serializer.WriteKey("Type").Write(AssetTypeToString(asset.Type));
		auto name = GetAssetName(asset);
		if(name != "")
			serializer.WriteKey("Name").Write(name);

		std::string path = GetPath(asset.ID);
		if(path != "")
			serializer.WriteKey("Path")
				.Write(fs::relative(path, rootPath).generic_string());
		else {
			Ref<Mesh> mesh = m_MeshAssets[asset.ID];
			List<Asset>& refs = m_References[asset.ID];
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

template<>
BinaryWriter& BinaryWriter::WriteObject(const Vec4& vec) {
	WriteData(&vec.x, sizeof(Vec4));

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

	BinaryWriter meshFile((assetPath / "Mesh" / "mesh.bin").string());
	BinaryWriter textureFile((assetPath / "Texture" / "image.bin").string());
	BinaryWriter soundFile((assetPath / "Audio" / "sound.bin").string());
	BinaryWriter scriptFile((assetPath / "Script" / "script.bin").string());

	BinaryWriter pack((fs::path(exportPath) / ".volc.assetpk").string());
	pack.Write(std::string("VOLC_PACK"));
	pack.Write(m_AssetRegistry.size());

	for(auto [asset, _] : m_AssetRegistry) {
		pack.Write((uint64_t)asset.ID);
		pack.Write((uint32_t)asset.Type);
		auto name = GetAssetName(asset);
		if(name != "") {
			pack.Write(true);
			pack.Write(name);
		}
		else
			pack.Write(false);
		auto path = m_Paths[asset.ID];

		if(asset.Type == AssetType::Mesh) {
			pack.Write(meshFile.GetPosition());
			Load(asset);
			auto mesh = Get<Mesh>(asset);

			if(path != "") {
				List<MaterialPaths> materials;
				List<SubMesh> meshes;
				AssetImporter::GetMeshData(path, meshes, materials);
				meshFile.Write(meshes);
				meshFile.Write(materials.Count());
				for(auto& mat : materials) {
					std::bitset<3> flags;
					flags |= (mat.Diffuse != "") << 0;
					flags |= (mat.Specular != "") << 1;
					flags |= (mat.Emissive != "") << 2;
					meshFile.Write((uint8_t)flags.to_ulong());
				}
				for(uint64_t i = 0; i < materials.Count(); i++) {
					meshFile.Write(mesh->Materials[i].DiffuseColor);
					meshFile.Write(mesh->Materials[i].SpecularColor);
					meshFile.Write(mesh->Materials[i].EmissiveColor);
				}
			}
			else {
				meshFile.Write(mesh->SubMeshes);
				meshFile.Write((uint64_t)0);
				meshFile.Write(mesh->Materials[0].DiffuseColor);
				meshFile.Write(mesh->Materials[0].SpecularColor);
				meshFile.Write(mesh->Materials[0].EmissiveColor);
			}
		}
		else if(asset.Type == AssetType::Texture) {
			pack.Write(textureFile.GetPosition());

			ImageData image = AssetImporter::GetImageData(path, false);
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

		if(!HasRefs(asset))
			pack.Write((uint64_t)0);
		else {
			auto& refs = m_References[asset.ID];
			pack.Write(refs.Count());
			for(auto& ref : refs) {
				pack.Write((uint64_t)ref.ID);
				pack.Write((uint32_t)ref.Type);
			}
		}
	}

	Application::PushDir();

	for(auto name : { "Framebuffer", "Lighting", "Bloom", "Mesh" }) {
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