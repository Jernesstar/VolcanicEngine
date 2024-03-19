#pragma once

#include <vector>
#include <string>

namespace VolcaniCore {

enum class BufferDataType { Float, Int, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4 };

static uint32_t BufferElementSize(BufferDataType type)
{
    switch(type)
    {
        case BufferDataType::Float: return 4;
        case BufferDataType::Int:   return 4;
        case BufferDataType::Vec2:  return 4 * 2;
        case BufferDataType::Vec3:  return 4 * 3;
        case BufferDataType::Vec4:  return 4 * 4;
        case BufferDataType::Mat2:  return 4 * 2 * 2;
        case BufferDataType::Mat3:  return 4 * 3 * 3;
        case BufferDataType::Mat4:  return 4 * 4 * 4;
    }
    return 0;
}

static uint32_t BufferElementCount(BufferDataType type)
{
    switch(type)
    {
        case BufferDataType::Float: return 1;
        case BufferDataType::Int:   return 1;
        case BufferDataType::Vec2:  return 2;
        case BufferDataType::Vec3:  return 3;
        case BufferDataType::Vec4:  return 4;
        case BufferDataType::Mat2:  return 2; // 2 * Vec2
        case BufferDataType::Mat3:  return 3; // 3 * Vec3
        case BufferDataType::Mat4:  return 4; // 4 * Vec4
    }
    return 0;
}

struct BufferElement {
    const std::string Name;
    const BufferDataType Type;
    const uint32_t Size;
    const uint32_t ComponentCount;
    const bool Normalized;

    BufferElement(const std::string& name, BufferDataType type, bool normalized = true)
        : Name(name), Type(type), Size(BufferElementSize(type)),
            ComponentCount(BufferElementCount(type)), Normalized(normalized) { }
};

class BufferLayout {
public:
    const std::vector<BufferElement> Elements;
    const uint32_t Stride;
    const bool Dynamic;

public:
    BufferLayout(const std::initializer_list<BufferElement>& elements, bool dynamic = true)
        : Elements(elements), Stride(CalculateStride(elements)), Dynamic(dynamic) { }

    std::vector<BufferElement>::const_iterator begin() const { return Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return Elements.end(); }

private:
    uint32_t CalculateStride(const std::initializer_list<BufferElement>& elements)
    {
        int stride = 0;
        for(auto& element : elements)
            stride += element.Size;
        return stride;
    }
};

}