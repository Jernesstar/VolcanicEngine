#pragma once

#include "Object/Quad.h"
#include "Object/Text.h"
#include "Object/Texture.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Transform.h"

namespace VolcaniCore {

class Renderer2D {
public:
	static void Begin(Ref<OrthographicCamera> camera);
	static void End();

	static void DrawQuad(Ref<Quad> texture,		const Transform& t = { });
	static void DrawQuad(Ref<Texture> quad,		const Transform& t = { });
	static void DrawQuad(const glm::vec4& color, const Transform& t = { });

	static void DrawText(Ref<Text> text, const Transform& t = { });
};

}