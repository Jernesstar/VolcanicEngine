#pragma once

#include <vector>
#include <string>
#include <cstdint>

// TODO(Maybe): Move to VolcaniCore
namespace VolcaniCore::OpenGL {

enum class BufferDataType { Int, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4 };

struct BufferElement {
public:
	const std::string Name;
	const BufferDataType Type;
	const bool Normalized;
	const uint32_t Size;
	const uint32_t Count;

	BufferElement(const std::string& name, BufferDataType type,
				  bool normalized = true)
		: Name(name), Type(type), Normalized(normalized),
			Size(CalcSize(type)), Count(CalcCount(type)) { }

private:
	static uint32_t CalcSize(BufferDataType type);
	static uint32_t CalcCount(BufferDataType type);
};

class BufferLayout {
public:
	const std::vector<BufferElement> Elements;
	const uint32_t Stride;
	const bool Dynamic;
	const bool StructureOfArrays;

public:
	BufferLayout(const std::initializer_list<BufferElement>& elements,
				 bool dynamic = true, bool structureOfArrays = false)
		: Elements(elements), Stride(CalcStride(elements)),
			Dynamic(dynamic), StructureOfArrays(structureOfArrays) { }

	std::vector<BufferElement>::const_iterator begin() const {
		return Elements.begin();
	}
	std::vector<BufferElement>::const_iterator end() const {
		return Elements.end();
	}

private:
	uint32_t CalcStride(const std::initializer_list<BufferElement>& elements) {
		uint32_t stride = 0;
		for(auto& element : elements)
			stride += element.Size;
		return stride;
	}
};

}