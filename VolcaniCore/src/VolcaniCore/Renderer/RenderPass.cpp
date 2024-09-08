#include "RenderPass.h"

#define GET_UNIFORMS(T, Uniform) \
template<> \
UniformMap<T>& Handles::GetUniforms<T>() { \
	return Uniform##Handles; \
}

#define SET_UNIFORM(TUniform) \
for(auto& [uniformName, valueCallback] : m_Handles.TUniform##Handles) \
	m_Pipeline->Set##TUniform(uniformName, valueCallback());

namespace VolcaniCore {

GET_UNIFORMS(uint32_t, Int)
GET_UNIFORMS(float, Float)
GET_UNIFORMS(TextureSlot, Texture)
GET_UNIFORMS(glm::vec2, Vec2)
GET_UNIFORMS(glm::vec3, Vec3)
GET_UNIFORMS(glm::vec4, Vec4)
GET_UNIFORMS(glm::mat2, Mat2)
GET_UNIFORMS(glm::mat3, Mat3)
GET_UNIFORMS(glm::mat4, Mat4)

void RenderPass::SetUniforms() {
	m_Pipeline->Bind();

	SET_UNIFORM(Int);
	SET_UNIFORM(Float);
	for(auto& [uniform, callbackValue] : m_Handles.TextureHandles) {
		auto slot = callbackValue();
		m_Pipeline->SetTexture(uniform, slot.Sampler, slot.Index);
	}

	SET_UNIFORM(Vec2);
	SET_UNIFORM(Vec3);
	SET_UNIFORM(Vec4);

	SET_UNIFORM(Mat2);
	SET_UNIFORM(Mat3);
	SET_UNIFORM(Mat4);
}

}