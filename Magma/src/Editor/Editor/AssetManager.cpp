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
	
}

void EditorAssetManager::Save(const std::string& path) {
	YAMLSerializer serializer;

	for(auto& [id, model] : m_MeshAssets) {
		if(model->Path != "") {
			serializer.WriteKey("Model")
			.BeginMapping()
				.WriteKey("Path").Write(model->Path)
			.EndMapping();
		}
		else {
			serializer.WriteKey("Meshes").BeginSequence();

			for(auto mesh : *model) {
				serializer.BeginMapping();
				serializer.WriteKey("Mesh").BeginMapping();

				if(mesh->Path != "")
					serializer.WriteKey("Path").Write(mesh->Path);
				else {
					serializer.WriteKey("Vertices")
					.SetOptions(Serializer::Options::ArrayOneLine)
					.Write(mesh->GetVertices());

					serializer.WriteKey("Indices")
					.SetOptions(Serializer::Options::ArrayOneLine)
					.Write(mesh->GetIndices());

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
		}
	}
}

void EditorAssetManager::SaveRuntime(const std::string& path) {

}

}