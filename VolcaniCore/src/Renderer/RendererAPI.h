#pragma once

#include "Core/Defines.h"

#include "Model.h"
#include "Quad.h"
#include "Text.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Transform.h"

namespace VolcaniCore {

enum class RenderAPI { OpenGL, Vulkan, DirectX, bgfx };

class RendererAPI {
public:
	// const RenderAPI API;

public:
	// RendererAPI(RenderAPI api);

	virtual void Init() = 0;
	virtual void Close() = 0;

	virtual void Clear() = 0;

	virtual void RenderModel(Ref<Model> model) = 0;
	virtual void RenderCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void RenderQuad(Ref<Quad> quad, Transform t) = 0;
	virtual void RenderText(Ref<Text> text, Transform t) = 0;
	virtual void RenderTexture(Ref<Texture> texture, Transform t) = 0;

	static Ref<RendererAPI> CreateRenderer(RenderAPI api);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}