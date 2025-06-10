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

	GridSet& operator =(const GridSet& other);

	void Resize(uint32_t width, uint32_t height);
	void ResizeX(uint32_t width);
	void ResizeY(uint32_t height);
	void Clear();

	uint8_t* Get() const;
	uint8_t* At(uint32_t x, uint32_t y) const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetCount() const;

	operator bool() const { return m_Data != nullptr; }

private:
	uint8_t* m_Data = nullptr;
	uint32_t m_Width = 0, m_Height = 0;

private:
	void Reallocate(uint32_t width, uint32_t height);
};

}