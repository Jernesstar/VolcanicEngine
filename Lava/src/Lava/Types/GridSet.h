#pragma once

#include <VolcaniCore/Core/List.h>

namespace Lava {

class GridSet {
public:
	static void RegisterInterface();

public:
	GridSet() = default;
	GridSet(uint32_t width, uint32_t height);
	~GridSet();

	void ResizeX(uint32_t width);
	void ResizeY(uint32_t height);
	void Clear();

	uint8_t* At(uint32_t x, uint32_t y) const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetCount() const;

private:
	uint8_t* m_Data = nullptr;
	uint32_t m_Width = 0, m_Height = 0;
};

}