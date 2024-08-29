#include "RenderPass.h"

#define GET_HANDLES(T, Uniform) \
template<> \
HandleMap<T>& Handles::GetHandles<T>() { \
	return Uniform##Handles; \
}

#define SET_UNIFORM(TUniform) \
for(auto& [uniformName, valueCallback] : m_Handles.TUniform##Handles) \
	m_Pipeline->Set##TUniform(uniformName, valueCallback());

namespace VolcaniCore {

GET_HANDLES(uint32_t, Int)
GET_HANDLES(float, Float)
// GET_HANDLES(Texture)
GET_HANDLES(glm::vec2, Vec2)
GET_HANDLES(glm::vec3, Vec3)
GET_HANDLES(glm::vec4, Vec4)
GET_HANDLES(glm::mat2, Mat2)
GET_HANDLES(glm::mat3, Mat3)
GET_HANDLES(glm::mat4, Mat4)

void RenderPass::LinkHandles() {
	m_Pipeline->Bind();

	SET_UNIFORM(Int);
	SET_UNIFORM(Float);
	// SET_UNIFORM(Texture);

	SET_UNIFORM(Vec2);
	SET_UNIFORM(Vec3);
	SET_UNIFORM(Vec4);

	SET_UNIFORM(Mat2);
	SET_UNIFORM(Mat3);
	SET_UNIFORM(Mat4);

	// for(auto& [uniform, callbackValue] : m_Handles.TextureHandles) {
	// 	auto& [texture, slot] = callbackValue();
	// 	m_Pipeline->SetTexture(uniform, texture, slot);
	// }
}

}