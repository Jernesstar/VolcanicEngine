#include "AssetManager.h"

#include "Core/YAMLSerializer.h"

namespace Magma {

EditorAssetManager::EditorAssetManager() {

}

EditorAssetManager::~EditorAssetManager() {

}

void EditorAssetManager::Load(Asset& asset) {

}

void EditorAssetManager::Unload(Asset& asset) {

}

Asset EditorAssetManager::Get(const std::string& path, AssetType type) {

}

void EditorAssetManager::Load(const std::string& path) {
	m_Path = path;

	
}

void EditorAssetManager::Save(const std::string& path) {
	YAMLSerializer serializer;

	serializer.WriteKey("MeshAssets").BeginSequence();
	for(auto& [id, model] : m_MeshAssets) {
		if(model->Path != "") {

		}
		else {
			serializer.BeginMapping();
			serializer.WriteKey("Mesh").BeginMapping();
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

			serializer.EndMapping(); // Mesh
			serializer.EndMapping();
		}
	}

	serializer.EndSequence();
}

void EditorAssetManager::SaveRuntime(const std::string& path) {

}

}