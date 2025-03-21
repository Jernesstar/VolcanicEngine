#include "RenderPass.h"

#include "Core/Assert.h"

#define GET_HANDLES(TUniform, Uniform) \
template<> \
CallbackMap<TUniform>& Uniforms::GetCallbacks<TUniform>() { \
	return Uniform##Callbacks; \
}

#define SET_UNIFORM(TUniform) \
for(auto& [uniformName, valueCallback] : m_Uniforms.TUniform##Callbacks) \
	uniforms.SetInput(uniformName, valueCallback());

namespace VolcaniCore {

void RenderPass::SetUniforms(DrawCommand* command) {
	auto& uniforms = command->UniformData;

	SET_UNIFORM(Int);
	SET_UNIFORM(Float);
	for(auto& [uniformName, callbackValue] : m_Uniforms.TextureCallbacks) {
		auto slot = callbackValue();
		if(slot.Sampler)
			uniforms.SetInput(uniformName, slot);
	}

	SET_UNIFORM(Vec2);
	SET_UNIFORM(Vec3);
	SET_UNIFORM(Vec4);

	SET_UNIFORM(Mat2);
	SET_UNIFORM(Mat3);
	SET_UNIFORM(Mat4);
}

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