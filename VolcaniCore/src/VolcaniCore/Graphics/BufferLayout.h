#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

enum class BufferDataType { Int, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4 };

struct BufferElement {
	const std::string Name;
	const BufferDataType Type;
	const bool Normalized;
	const uint32_t Size;
	const uint32_t Count;

	BufferElement(const std::string& name, BufferDataType type,
				  bool normalized = true)
		: Name(name), Type(type), Normalized(normalized),
			Size(CalcSize(type)), Count(CalcCount(type)) { }

	bool operator ==(const BufferElement& other) const {
		return Type == other.Type
			&& Normalized == other.Normalized
			&& Size == other.Size
			&& Count == other.Count;
	}

private:
	static uint32_t CalcSize(BufferDataType type) {
		switch(type) {
			case BufferDataType::Int:	return 4;
			case BufferDataType::Float: return 4;
			case BufferDataType::Vec2:	return 4 * 2;
			case BufferDataType::Vec3:	return 4 * 3;
			case BufferDataType::Vec4:	return 4 * 4;
			case BufferDataType::Mat2:	return 4 * 2 * 2;
			case BufferDataType::Mat3:	return 4 * 3 * 3;
			case BufferDataType::Mat4:	return 4 * 4 * 4;
		}

		return 0;
	}
	static uint32_t CalcCount(BufferDataType type) {
		switch(type) {
			case BufferDataType::Int:	return 1;
			case BufferDataType::Float: return 1;
			case BufferDataType::Vec2:	return 2;
			case BufferDataType::Vec3:	return 3;
			case BufferDataType::Vec4:	return 4;
			case BufferDataType::Mat2:	return 2; // 2 * Vec2
			case BufferDataType::Mat3:	return 3; // 3 * Vec3
			case BufferDataType::Mat4:	return 4; // 4 * Vec4
		}

		return 0;
	}
};

class BufferLayout {
public:
	const List<BufferElement> Elements;
	const uint32_t Stride;
	const bool Dynamic;
	const bool StructureOfArrays;

public:
	BufferLayout(const std::initializer_list<BufferElement>& elements,
				 bool dynamic = true, bool structureOfArrays = false)
		: Elements(elements), Stride(CalcStride(elements)),
			Dynamic(dynamic), StructureOfArrays(structureOfArrays) { }

	bool operator ==(const BufferLayout& other) const {
		return Elements == other.Elements
			&& Stride == other.Stride
			&& Dynamic == other.Dynamic
			&& StructureOfArrays == other.StructureOfArrays;
	}

	List<BufferElement>::const_iterator begin() const {
		return Elements.begin();
	}
	List<BufferElement>::const_iterator end() const {
		return Elements.end();
	}

private:
	uint32_t CalcStride(const List<BufferElement>& elements) {
		uint32_t stride = 0;
		for(auto& element : elements)
			stride += element.Size;
		return stride;
	}
};

}