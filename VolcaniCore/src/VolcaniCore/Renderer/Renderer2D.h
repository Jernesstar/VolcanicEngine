#pragma once

#include "Object/Quad.h"
#include "Object/Text.h"
#include "Object/Texture.h"

#include "Renderer/OrthographicCamera.h"

namespace VolcaniCore {


class Renderer2D {
public:
	static void Begin(Ref<OrthographicCamera> camera);
	static void End();

	static void DrawQuad(Ref<Quad> texture,		 Transform t = { });
	static void DrawQuad(Ref<Texture> quad,		 Transform t = { });
	static void DrawQuad(const glm::vec4& color, Transform t = { });

	static void DrawText(Ref<Text> text, Transform t = { });

private:
	static void Init();
	static void Close();

	static void Flush();

	friend class Renderer;
};


}