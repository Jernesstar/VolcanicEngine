#pragma once

#include "Graphics/OrthographicCamera.h"
#include "Graphics/Transform.h"

#include "Graphics/Quad.h"
#include "Graphics/Text.h"
#include "Graphics/Texture.h"
#include "Graphics/Framebuffer.h"

namespace VolcaniCore {

class Renderer2D {
public:
	static void StartFrame();
	static void EndFrame();

	static void Begin(Ref<OrthographicCamera> camera);
	static void End();

	static void DrawQuad(Ref<Quad> texture, const Transform& t = { });
	static void DrawQuad(Ref<Texture> quad, const Transform& t = { });
	static void DrawQuad(const glm::vec4& color, const Transform& t = { });

	static void DrawText(Ref<Text> text, const Transform& t = { });

	static void DrawFullscreenQuad(Ref<Framebuffer> buffer,
								   AttachmentTarget target);

private:
	static void Init();
	static void Close();

	friend class Renderer;
};

}