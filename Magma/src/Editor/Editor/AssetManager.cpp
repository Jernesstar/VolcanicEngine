#include "AssetManager.h"

#include "Core/YAMLSerializer.h"

namespace Magma {

EditorAssetManager::EditorAssetManager() {

}

EditorAssetManager::~EditorAssetManager() {

}

void EditorAssetManager::Load(Asset& asset) {
	auto path = m_Paths[asset.ID];
	m_MeshAssets[asset.ID] = Model::Create(path);
}

void EditorAssetManager::Unload(Asset& asset) {

}

Asset EditorAssetManager::Get(const std::string& path, AssetType type) {

}

void EditorAssetManager::Load(const std::string& path) {
	if(!FileUtils::FileExists(path))
		return;
	m_Path = path;

	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								path.c_str(), e.what());
	}
	auto assetPackNode = file["AssetPack"];

	
}

void EditorAssetManager::Save(const std::string& path) {
	YAMLSerializer serializer;
	serializer.BeginMapping();

	serializer.WriteKey("AssetPack").BeginMapping();

	serializer.WriteKey("MeshAssets").BeginSequence();
	for(auto& [id, model] : m_MeshAssets) {
		serializer.BeginMapping();
		serializer.WriteKey("Mesh").BeginMapping();

		if(model->Path != "")
			serializer.WriteKey("Path").Write(model->Path);
		else {
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

	serializer.EndMapping(); // AssetPack
	serializer.EndMapping();
}

void EditorAssetManager::SaveRuntime(const std::string& path) {

}

}