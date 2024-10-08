#include "RenderPass.h"

#include "Core/Assert.h"

#define GET_HANDLES(TUniform, Uniform) \
template<> \
HandleMap<TUniform>& Uniforms::GetHandles<TUniform>() { \
	return Uniform##Handles; \
}

#define SET_UNIFORM(TUniform) \
for(auto& [uniformName, valueCallback] : uniforms.TUniform##Handles) \
	m_Pipeline->Set##TUniform(uniformName, valueCallback());

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

void RenderPass::SetUniforms(const Uniforms& uniforms) {
	SET_UNIFORM(Int);
	SET_UNIFORM(Float);
	for(auto& [uniformName, callbackValue] : uniforms.TextureHandles) {
		auto slot = callbackValue();
		if(slot.Sampler)
			m_Pipeline->SetTexture(uniformName, slot.Sampler, slot.Index);
	}

	SET_UNIFORM(Vec2);
	SET_UNIFORM(Vec3);
	SET_UNIFORM(Vec4);

	SET_UNIFORM(Mat2);
	SET_UNIFORM(Mat3);
	SET_UNIFORM(Mat4);
}

void RenderPass::SetGlobalUniforms()
{
	m_Pipeline->Bind();

	SetUniforms(m_GlobalUniforms);
}
}