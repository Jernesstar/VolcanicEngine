#include "RenderPass.h"

#define GET_HANDLES(TUniform) \
template<> \
HandleMap<TUniform>& RenderPass::GetHandles<TUniform>() { \
	return TUniform##Handles; \
}

#define SET_UNIFORM(TUniform) \
for(auto& [uniformName, valueCallback] : m_Handles.##TUniform##Handles) \
	m_Pipeline->Set##TUniform##(uniformName, valueCallback());

namespace VolcaniCore {

GET_HANDLES(Int)
GET_HANDLES(Float)
// GET_HANDLES(Texture)
GET_HANDLES(Vec2)
GET_HANDLES(Vec3)
GET_HANDLES(Vec4)
GET_HANDLES(Mat2)
GET_HANDLES(Mat3)
GET_HANDLES(Mat4)

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

	for(auto& [uniform, callbackValue] : m_Handles.TextureHandles) {
		auto& [texture, slot] = callbackValue();
		m_Pipeline->SetTexture(uniform, texture, slot);
	}
}

}