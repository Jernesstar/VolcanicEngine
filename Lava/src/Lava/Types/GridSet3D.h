#pragma once

#include <VolcaniCore/Core/List.h>

namespace Lava {

class GridSet3D {
public:
	static void RegisterInterface();

public:
	GridSet3D() = default;
	GridSet3D(uint32_t length, uint32_t width, uint32_t height);
	~GridSet3D();

	void ResizeX(uint32_t length);
	void ResizeY(uint32_t width);
	void ResizeZ(uint32_t height);
	void Clear();

	uint8_t* At(uint32_t x, uint32_t y, uint32_t z) const;
	uint32_t GetCount() const;

private:
	uint8_t* m_Data = nullptr;
	uint32_t m_Length = 0, m_Width = 0, m_Height = 0;
};

}