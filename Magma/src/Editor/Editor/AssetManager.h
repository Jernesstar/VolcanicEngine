#pragma once

#include <Magma/Core/AssetManager.h>

namespace Magma {

class EditorAssetManager : public AssetManager {
public:
	EditorAssetManager();
	~EditorAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;

	Asset Add(const std::string& path, AssetType type);

	UUID GetFromPath(const std::string& path);

	std::string GetPath(UUID id);

	const Map<Asset, bool>& GetRegistry() const { return m_AssetRegistry; }

	void Load(const std::string& path);
	void Reload();

	void Save();
	void RuntimeSave(const std::string& exportPath);

public:
	std::string m_Path;
	Map<UUID, std::string> m_Paths;
	Map<UUID, Asset[6]> m_References;
};

}