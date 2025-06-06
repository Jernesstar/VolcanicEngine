#pragma once

#include <Magma/Core/AssetManager.h>
#include <Magma/Core/BinaryReader.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class ByteCodeReader : public asIBinaryStream {
public:
	ByteCodeReader(BinaryReader* reader)
		: m_Reader(reader) { }
	~ByteCodeReader() = default;

	int Read(void* data, uint32_t size) override {
		m_Reader->ReadData(data, (uint64_t)size);
		return 0;
	}

	int Write(const void* data, uint32_t size) override {
		return 0;
	}

private:
	BinaryReader* m_Reader = nullptr;
};

class RuntimeAssetManager : public AssetManager {
public:
	RuntimeAssetManager();
	~RuntimeAssetManager();

	void Load(Asset asset) override;
	void Unload(Asset asset) override;

	void Load();

private:
	Map<UUID, uint64_t> m_AssetOffsets;
};

}