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
#include "ScriptManager.h"

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
		case AssetType::Shader:
			return "Shader";
		case AssetType::Font:
			return "Font";
		case AssetType::Audio:
			return "Audio";
		case AssetType::Script:
			return "Script";
		case AssetType::Material:
			return "Material";
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
	else if(str == "Shader")
		return AssetType::Shader;
	else if(str == "Font")
		return AssetType::Font;
	else if(str == "Audio")
		return AssetType::Audio;
	else if(str == "Script")
		return AssetType::Script;

	return AssetType::None;
}

class FileWatcher : public efsw::FileWatchListener {
public:
	FileWatcher(EditorAssetManager* assetManager);
	~FileWatcher() = default;

	void handleFileAction(efsw::WatchID id, const std::string& dir,
		const std::string& file, efsw::Action action, std::string old) override;

	uint32_t AddCallback(const Func<void, Asset, bool>& callback);
	void RemoveCallback(uint32_t id);

	void Add(AssetType type, const std::string& path);
	void Remove(AssetType type, const std::string& path);

	void ReloadMesh(const std::string& path);
	void ReloadTexture(const std::string& path);
	void ReloadShader(const std::string& path);
	void ReloadAudio(const std::string& path);
	void ReloadScript(const std::string& path);

private:
	EditorAssetManager* m_AssetManager;
	List<Func<void, Asset, bool>> m_Callbacks;
};

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
		for(auto callback : m_Callbacks)
			callback(asset, 0);

		switch(type) {
			case AssetType::Mesh:
				ReloadMesh(fullPath);
				break;
			case AssetType::Texture:
				ReloadTexture(fullPath);
				break;
			// case AssetType::Cubemap:
			// 	ReloadCubemap(fullPath);
			// 	break;
			case AssetType::Shader:
				ReloadShader(fullPath);
				break;
			// case AssetType::Font:
			// 	ReloadFont(fullPath);
			// 	break;
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
	else if(action == efsw::Actions::Moved) {
		VOLCANICORE_LOG_INFO("%s has moved from %s",
			file.c_str(), oldFilename.c_str());
		// m_AssetManager->Change(fullPath, asset);
	}
	else {
		VOLCANICORE_LOG_ERROR("Unexpected filewatch error!");
	}
}

uint32_t FileWatcher::AddCallback(const Func<void, Asset, bool>& callback) {
	m_Callbacks.Add(callback);
	return m_Callbacks.Count() - 1;
}

void FileWatcher::RemoveCallback(uint32_t id) {
	m_Callbacks.Pop(id);
}

void FileWatcher::Add(AssetType type, const std::string& path) {
	VOLCANICORE_LOG_INFO("Adding AssetType::%s at path '%s'",
		AssetTypeToString(type).c_str(), path.c_str());
	m_AssetManager->Add(type, 0, true, path);
}

void FileWatcher::Remove(AssetType type, const std::string& path) {
	VOLCANICORE_LOG_INFO("Removing AssetType::%s at path '%s'",
		AssetTypeToString(type).c_str(), path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	m_AssetManager->Remove({ id, type });
}

void FileWatcher::ReloadMesh(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Mesh at path '%s'", path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	m_AssetManager->Unload({ id, AssetType::Mesh });
	m_AssetManager->Load({ id, AssetType::Mesh });
}

void FileWatcher::ReloadTexture(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Texture at path '%s'", path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	m_AssetManager->Unload({ id, AssetType::Texture });
	m_AssetManager->Load({ id, AssetType::Texture });
}

void FileWatcher::ReloadShader(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Shader at path '%s'", path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	m_AssetManager->Unload({ id, AssetType::Shader });
	m_AssetManager->Load({ id, AssetType::Shader });
}

void FileWatcher::ReloadAudio(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Audio at path '%s'", path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	m_AssetManager->Unload({ id, AssetType::Audio });
	m_AssetManager->Load({ id, AssetType::Audio });
}

void FileWatcher::ReloadScript(const std::string& path) {
	VOLCANICORE_LOG_INFO("Reloading Script at path '%s'", path.c_str());
	UUID id = m_AssetManager->GetFromPath(path);
	bool error;
	auto mod = AssetImporter::GetScriptData(path, &error, "TestBuild");

	if(!error) {
		m_AssetManager->Unload({ id, AssetType::Script });
		m_AssetManager->Load({ id, AssetType::Script });
	}
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

	if(asset.Type == AssetType::Mesh) {
		if(path != "")
			m_MeshAssets[asset.ID] = AssetImporter::GetMesh(path);
	}
	else if(asset.Type == AssetType::Texture)
		m_TextureAssets[asset.ID] = AssetImporter::GetTexture(path);
	else if(asset.Type == AssetType::Cubemap)
		m_CubemapAssets[asset.ID] = AssetImporter::GetCubemap(path);
	else if(asset.Type == AssetType::Shader) {
		auto folder = fs::path(path).parent_path().string();
		auto name = fs::path(path).filename().string();
		List<std::string> shaders;
		for(auto ref : m_References[asset.ID])
			shaders.Add(GetPath(ref.ID));
		m_ShaderAssets[asset.ID] = AssetImporter::GetShader(shaders);
	}
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets[asset.ID] = AssetImporter::GetAudio(path);
	else if(asset.Type == AssetType::Script)
		m_ScriptAssets[asset.ID] = AssetImporter::GetScript(path);
	else if(asset.Type == AssetType::Material) {
		// Nothing, materials are always loaded
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
	else if(asset.Type == AssetType::Shader)
		m_ShaderAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Audio)
		m_AudioAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Script)
		m_ScriptAssets.erase(asset.ID);
	else if(asset.Type == AssetType::Material)
		m_MaterialAssets.erase(asset.ID);
}

uint32_t EditorAssetManager::AddReloadCallback(
	const Func<void, Asset, bool>& callback)
{
	return s_Listener->AddCallback(callback);
}

void EditorAssetManager::RemoveReloadCallback(uint32_t id) {
	s_Listener->RemoveCallback(id);
}

Asset EditorAssetManager::Add(AssetType type, UUID id, bool primary,
							  const std::string& path)
{
	Asset newAsset{ id ? id : UUID(), type, primary };
	m_AssetRegistry[newAsset] = false;
	if(path != "")
		m_Paths[newAsset.ID] = path;

	if(type == AssetType::Mesh && path != "") {
		List<SubMesh> meshes;
		List<MaterialPaths> materials;
		AssetImporter::GetMeshData(path, meshes, materials);
		for(auto matPaths : materials) {
			Asset material = Add(AssetType::Material, 0, false);
			for(uint32_t i = 0; i < 3; i++) {
				auto path = matPaths[i];
				if(path == "")
					continue;

				AddRef(material, Add(AssetType::Texture, 0, false, path));
			}

			AddRef(newAsset, material);
		}
	}
	else if(type == AssetType::Material)
		m_MaterialAssets[newAsset.ID] = CreateRef<Material>();

	return newAsset;
}

void EditorAssetManager::Remove(Asset asset) {
	Unload(asset);

	m_AssetRegistry.erase(asset);
	m_NamedAssets.erase(asset);
	m_References.erase(asset.ID);
	m_Paths.erase(asset.ID);
}

UUID EditorAssetManager::GetFromPath(const std::string& path) const {
	for(auto& [id, assetPath] : m_Paths)
		if(fs::path(path) == fs::path(assetPath))
			return id;

	return 0;
}

std::string EditorAssetManager::GetPath(UUID id) const {
	if(!m_Paths.count(id))
		return "";
	return m_Paths.at(id);
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

	fs::path assetDir;
	assetDir = rootPath / "Mesh";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Image";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Cubemap";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Shader";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Font";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Audio";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));
	assetDir = rootPath / "Script";
	if(fs::exists(assetDir))
		s_WatcherIDs.Add(
			s_FileWatcher->addWatch(assetDir.string(), s_Listener));

	s_FileWatcher->watch();

	auto assetPackNode = file["AssetPack"];
	if(!assetPackNode)
		return;

	for(auto assetNode : assetPackNode["Assets"]) {
		auto node = assetNode["Asset"];
		AssetType type = AssetTypeFromString(node["Type"].as<std::string>());
		UUID id = node["ID"].as<uint64_t>();
		std::string path = "";
		if(node["Path"]) {
			path = (rootPath / node["Path"].as<std::string>()).generic_string();
			if(!fs::exists(path))
				continue;
		}

		Asset asset = Add(type, id, true, path);

		if(type == AssetType::Mesh && !node["Path"]) {
			auto type = (MeshType)node["MeshType"].as<uint32_t>();
			auto materialID = node["MaterialID"].as<uint64_t>();
			m_MeshAssets[id] = Mesh::Create(type);

			AddRef(asset, { materialID, AssetType::Material, false });
		}
		else if(type == AssetType::Material) {
			// if(materialNode["Diffuse"])
			// 	mat.Diffuse =
			// 		AssetImporter::GetTexture(
			// 			materialNode["Diffuse"]["Path"].as<std::string>());
			// if(materialNode["Specular"])
			// 	mat.Specular =
			// 		AssetImporter::GetTexture(
			// 			materialNode["Specular"]["Path"].as<std::string>());
			// if(materialNode["Emissive"])
			// 	mat.Emissive =
			// 		AssetImporter::GetTexture(
			// 			materialNode["Emissive"]["Path"].as<std::string>());

			// mat.DiffuseColor = materialNode["DiffuseColor"].as<glm::vec4>();
			// mat.SpecularColor = materialNode["SpecularColor"].as<glm::vec4>();
			// mat.EmissiveColor = materialNode["EmissiveColor"].as<glm::vec4>();

		}

		if(node["Name"])
			NameAsset(asset, node["Name"].as<std::string>());
	}

	List<fs::path> paths
	{
		(rootPath / "Mesh"),
		(rootPath / "Image"),
		(rootPath / "Cubemap"),
		(rootPath / "Shader"),
		(rootPath / "Font"),
		(rootPath / "Audio"),
		(rootPath / "Script"),
	};

	uint32_t i = 1;
	for(auto& folder : paths) {
		for(auto path : FileUtils::GetFiles(folder.string())) {
			if(i == 1)
				path = FileUtils::GetFiles(path, { ".obj" })[0];
			if(!GetFromPath(path))
				Add((AssetType)i, 0, true, path);
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
			if(asset.Type == AssetType::Mesh) {
				Ref<Mesh> mesh = m_MeshAssets[asset.ID];
				const SubMesh& subMesh = mesh->SubMeshes[0];
				serializer.WriteKey("MeshType").Write((uint32_t)mesh->Type);

				const List<Asset>& refs = GetRefs(asset);
				serializer.WriteKey("MaterialID").Write((uint64_t)refs[0].ID);
			}
			else if(asset.Type == AssetType::Material) {
				// for(auto floatUniform : )

				// auto& mat = mesh->Materials[subMesh.MaterialIndex];
				// if(mat.Diffuse)
				// 	serializer.WriteKey("Diffuse")
				// 	.BeginMapping()
				// 		.WriteKey("Path").Write(m_Paths[refs[0].ID])
				// 	.EndMapping();

				// if(mat.Specular)
				// 	serializer.WriteKey("Specular")
				// 	.BeginMapping()
				// 		.WriteKey("Path").Write(m_Paths[refs[1].ID])
				// 	.EndMapping();

				// if(mat.Emissive)
				// 	serializer.WriteKey("Emissive")
				// 	.BeginMapping()
				// 		.WriteKey("Path").Write(m_Paths[refs[2].ID])
				// 	.EndMapping();

				// serializer
				// 	.WriteKey("DiffuseColor").Write(mat.DiffuseColor);
				// serializer
				// 	.WriteKey("SpecularColor").Write(mat.SpecularColor);
				// serializer
				// 	.WriteKey("EmissiveColor").Write(mat.EmissiveColor);

				// serializer.EndMapping();
			}
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
BinaryWriter& BinaryWriter::WriteObject(const Asset& asset) {
	Write((uint64_t)asset.ID);
	Write((uint8_t)asset.Type);
	Write(asset.Primary);

	Write(AssetManager::Get()->GetAssetName(asset));

	if(AssetManager::Get()->HasRefs(asset)) {
		const auto& refs = AssetManager::Get()->GetRefs(asset);
		Write(refs.Count());
		for(auto ref : refs) {
			Write((uint64_t)ref.ID);
			Write((uint8_t)ref.Type);
		}
	}
	else
		Write((uint64_t)0);

	return *this;
}

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

	BinaryWriter pack((fs::path(exportPath) / ".volc.assetpk").string());
	pack.Write(std::string("VOLC_PACK"));
	pack.Write((uint64_t)m_AssetRegistry.size());

	uint64_t objectIdx = pack.GetPosition();
	for(auto [asset, _] : m_AssetRegistry) {
		objectIdx += sizeof(uint64_t) + sizeof(uint8_t) + sizeof(bool); // ID, Type, Primary
		objectIdx += sizeof(uint64_t); // Object Index
	}
	for(auto [_, refs] : m_References) {
		objectIdx += sizeof(uint64_t); // RefCount
		objectIdx += (sizeof(uint64_t) + sizeof(uint8_t)) * refs.Count(); // (ID + Type) * RefCount
	}
	for(auto [_, name] : m_NamedAssets) {
		objectIdx += sizeof(uint64_t); // Name length
		objectIdx += sizeof(char) * name.size(); // Name
	}

	for(auto [asset, _] : m_AssetRegistry) {
		pack.Write(asset); // Asset data (type, id, primary, name, refs)
		pack.Write(objectIdx); // Where the Asset points to, the actual object

		auto registryPos = pack.GetPosition(); // Record current position
		pack.SetPosition(objectIdx); // Jump to object position

		auto path = GetPath(asset.ID);
		if(asset.Type == AssetType::Mesh) {
			Load(asset);
			auto mesh = Get<Mesh>(asset);

			pack.Write(mesh->SubMeshes);
		}
		else if(asset.Type == AssetType::Texture) {
			ImageData image = AssetImporter::GetImageData(path, false);
			pack.Write(image.Width);
			pack.Write(image.Height);
			pack.WriteData(image.Data.Get(), image.Data.GetSize());
		}
		else if(asset.Type == AssetType::Cubemap) {
			// Nothing, virtual asset that simply references other images
			pack.Write((uint32_t)4);
		}
		else if(asset.Type == AssetType::Shader) {
			Buffer<uint32_t> data = AssetImporter::GetShaderData(path);
			pack.Write(data);
		}
		else if(asset.Type == AssetType::Audio) {
			Buffer<float> soundData = AssetImporter::GetAudioData(path);
			pack.Write(soundData);
		}
		else if(asset.Type == AssetType::Script) {
			auto* mod = ScriptManager::LoadScript(path, false);
			pack.Write(std::string(mod->GetName()));
			ScriptManager::SaveScript(mod, pack);
			mod->Discard();
		}
		else if(asset.Type == AssetType::Material) {
			pack.Write((uint32_t)5);
		}

		objectIdx = pack.GetPosition(); // Object position for next asset
		pack.SetPosition(registryPos); // Return to registry position
	}

	Application::PushDir();

	fs::create_directories(fs::path(exportPath) / "Asset" / "Shader");
	for(auto name : { "Framebuffer", "Lighting", "Bloom", "Mesh" }) {
		auto sourceRoot = fs::path("Magma") / "assets" / "shaders" / name;
		auto source1 = sourceRoot.string() + ".glsl.vert";
		auto source2 = sourceRoot.string() + ".glsl.frag";

		auto targetRoot = fs::path(exportPath) / "Asset" / "Shader" / name;
		auto target1 = targetRoot.string() + ".bin.vert";
		auto target2 = targetRoot.string() + ".bin.frag";

		if(FileUtils::FileExists(target1))
			fs::remove(target1);
		if(FileUtils::FileExists(source1)) {
			BinaryWriter writer(target1);
			Buffer<uint32_t> data = AssetImporter::GetShaderData(source1);
			writer.Write(data);
		}

		if(FileUtils::FileExists(target2))
			fs::remove(target2);
		if(FileUtils::FileExists(source2)) {
			BinaryWriter writer(target2);
			Buffer<uint32_t> data = AssetImporter::GetShaderData(source2);
			writer.Write(data);
		}
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