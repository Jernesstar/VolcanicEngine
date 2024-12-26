#include "AssetManager.h"

#include "Graphics/Mesh.h"

namespace Magma {

static Map<std::string, Ref<Texture>> s_Images;
static Map<std::string, Ref<Mesh>> s_Meshes;

template<>
Ref<Mesh> Asset::Get() {
	VOLCANICORE_ASSERT(Type == AssetType::Mesh);
	return s_Meshes[Path];
}

void AssetManager::Init() {
	
}

void AssetManager::Close() {
	s_Images.clear();
	s_Meshes.clear();
}

// template<>
// Asset AssetManager::GetOrCreate(const std::string& path) {
// 	if(s_Meshes.find(path) == s_Meshes.end())
// 		s_Meshes[path] = Mesh::Create(path);

// 	return Asset(AssetType::Mesh, path);
// }

template<>
Ref<Mesh> AssetManager::GetOrCreate(const std::string& path) {
	if(s_Meshes.find(path) == s_Meshes.end())
		s_Meshes[path] = Mesh::Create(path);

	return s_Meshes[path];
}
}