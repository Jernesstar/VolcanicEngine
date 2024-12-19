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

void RenderPass::SetUniforms(DrawUniforms& uniformData) {
	for(auto& [uniformName, valueCallback] : m_Uniforms.IntHandles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.FloatHandles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.TextureHandles) {
		auto slot = valueCallback();
		if(slot.Sampler)
			uniformData.SetInput(uniformName, slot);
	}

	for(auto& [uniformName, valueCallback] : m_Uniforms.Vec2Handles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.Vec3Handles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.Vec4Handles)
		uniformData.SetInput(uniformName, valueCallback());

	for(auto& [uniformName, valueCallback] : m_Uniforms.Mat2Handles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.Mat3Handles)
		uniformData.SetInput(uniformName, valueCallback());
	for(auto& [uniformName, valueCallback] : m_Uniforms.Mat4Handles)
		uniformData.SetInput(uniformName, valueCallback());
}

}