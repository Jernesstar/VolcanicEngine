#pragma once

#include <Magma/Core/AssetManager.h>

namespace Magma {

extern std::string AssetTypeToString(AssetType type);
extern AssetType AssetTypeFromString(const std::string& str);

class EditorAssetManager : public AssetManager {
public:
	EditorAssetManager();
	~EditorAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;
	uint32_t AddReloadCallback(const Func<void, Asset, bool>& callback);
	void RemoveReloadCallback(uint32_t id);

	Asset Add(const std::string& path, AssetType type);
	void Remove(Asset asset);

	std::string GetPath(VolcaniCore::UUID id) const;
	VolcaniCore::UUID GetFromPath(const std::string& path) const;

	const auto& GetRegistry() const { return m_AssetRegistry; }

	void Load(const std::string& path);
	void Save();
	void RuntimeSave(const std::string& exportPath);

public:
	std::string m_Path;
	Map<VolcaniCore::UUID, std::string> m_Paths;
};

}