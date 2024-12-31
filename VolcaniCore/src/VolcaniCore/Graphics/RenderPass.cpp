#include "RenderPass.h"

#include "Core/Assert.h"

#define GET_HANDLES(TUniform, Uniform) \
template<> \
HandleMap<TUniform>& Uniforms::GetHandles<TUniform>() { \
	return Uniform##Handles; \
}

namespace VolcaniCore {

GET_HANDLES(int32_t, Int)
GET_HANDLES(float, Float)
GET_HANDLES(TextureSlot, Texture)
GET_HANDLES(glm::vec2, Vec2)
GET_HANDLES(glm::vec3, Vec3)
GET_HANDLES(glm::vec4, Vec4)
GET_HANDLES(glm::mat2, Mat2)
GET_HANDLES(glm::mat3, Mat3)
GET_HANDLES(glm::mat4, Mat4)

}