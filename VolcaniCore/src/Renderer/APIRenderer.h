#pragma once

#include "Core/Defines.h"

#include "Mesh.h"
#include "Quad.h"
#include "Text.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Transform.h"

namespace VolcaniCore {

class APIRenderer {
public:
	virtual void Init() = 0;
	virtual void Close() = 0;

	virtual void Clear() = 0;

	virtual void RenderMesh(Ref<Mesh> mesh) = 0;
	virtual void RenderCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void RenderQuad(Ref<Quad> quad, Transform t) = 0;
	virtual void RenderText(Ref<Text> text, Transform t) = 0;
	virtual void RenderTexture(Ref<Texture> texture, Transform t) = 0;

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}