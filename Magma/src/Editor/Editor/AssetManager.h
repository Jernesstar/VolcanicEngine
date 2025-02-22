#pragma once

#include <Magma/Core/AssetManager.h>

namespace Magma {

class EditorAssetManager : public AssetManager {
public:
	EditorAssetManager();
	~EditorAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;

	Asset Add(MeshPrimitive primitive);
	Asset Add(const std::string& path, AssetType type);

	Asset GetFromPath(const std::string& path);

	std::string GetPath(UUID id);

	template<typename T>
	Asset Find(Ref<T> data) {
		return { };
	}

	const Map<Asset, bool>& GetRegistry() const { return m_AssetRegistry; }

	void Load(const std::string& path);
	void Save();
	void RuntimeSave(const std::string& exportPath);

public:
	std::string m_Path;
	Map<UUID, MeshPrimitive> m_Primitives;
	Map<UUID, std::string> m_Paths;
};

}