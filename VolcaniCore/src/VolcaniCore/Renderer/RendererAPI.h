#pragma once

#include "Core/Defines.h"

#include "Object/Model.h"
#include "Object/Quad.h"
#include "Object/Text.h"
#include "Object/Texture.h"
#include "Object/Cubemap.h"
#include "Object/FrameBuffer.h"

#include "Renderer/RenderPass.h"
#include "Renderer/Transform.h"

namespace VolcaniCore {

enum class RendererBackend { OpenGL, Vulkan, DirectX, bgfx };

class RendererAPI {
public:
	const RendererBackend Backend;

public:
	RendererAPI(RendererBackend backend)
		: Backend(backend) { }
	virtual ~RendererAPI() = default;

	static void Create(RendererBackend backend);
	static Ref<RendererAPI> Get() { return s_Instance; }

	virtual void Init() = 0;
	virtual void Close() = 0;

	virtual void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual void Begin(Ref<Camera> camera) = 0;
	virtual void End() = 0;

	virtual void Render(Ref<RenderPass> pass) = 0;
	virtual void RenderFrameBuffer(Ref<FrameBuffer> buffer) = 0;

	virtual void DrawCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void Draw3DCube(Ref<Texture> texture,	Transform t = { }) = 0;
	virtual void Draw3DModel(Ref<Model> model,		Transform t = { }) = 0;

	virtual void Draw2DQuad(const glm::vec4& color, Transform t = { }) = 0;
	virtual void Draw2DQuad(Ref<Texture> texture,	Transform t = { }) = 0;
	virtual void Draw2DText(Ref<Text> text,			Transform t = { }) = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}