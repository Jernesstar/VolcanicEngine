#pragma once

#include <Magma/Core/AssetManager.h>

namespace Magma {

class EditorAssetManager : public AssetManager {
public:
	EditorAssetManager();
	~EditorAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;
	void AddReloadCallback(const Func<void, Asset>& callback);

	Asset Add(const std::string& path, AssetType type);

	std::string GetPath(VolcaniCore::UUID id);
	VolcaniCore::UUID GetFromPath(const std::string& path);

	const auto& GetRegistry() const { return m_AssetRegistry; }

	void Load(const std::string& path);
	void Save();
	void RuntimeSave(const std::string& exportPath);

public:
	std::string m_Path;
	Map<VolcaniCore::UUID, std::string> m_Paths;

	friend class FileWatcher;
};

}