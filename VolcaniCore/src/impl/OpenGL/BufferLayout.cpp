#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

uint32_t BufferElement::CalcSize(BufferDataType type) {
	switch(type) {
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

uint32_t BufferElement::CalcCount(BufferDataType type) {
	switch(type) {
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

}