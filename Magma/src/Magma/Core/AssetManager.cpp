#include "AssetManager.h"


namespace Magma {

static Map<std::string, Ref<Mesh>> s_Meshes;

void AssetManager::Init() {
	
}

void AssetManager::Close() {
	s_Meshes.clear();
}

template<>
Ref<Mesh> AssetManager::GetOrCreate(const std::string& path) {
	if(s_Meshes.find(path) == s_Meshes.end())
		s_Meshes[path] = Mesh::Create(path);

	return s_Meshes[path];
}


}