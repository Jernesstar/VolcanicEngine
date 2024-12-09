#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Transform.h"

#include "Object/Quad.h"
#include "Object/Text.h"
#include "Object/Texture.h"
#include "Object/Framebuffer.h"

namespace VolcaniCore {

class Renderer2D {
public:
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