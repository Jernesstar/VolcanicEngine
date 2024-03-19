#pragma once

#include "Renderer/APIRenderer.h"

namespace VolcaniCore {

class OpenGLRenderer : public APIRenderer {
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void Init();
	void ClearScreen();

	void RenderMesh(Ref<Mesh> mesh);
	void RenderCubemap(Ref<Cubemap> cubemap);
	void RenderQuad(Ref<Quad> quad, Transform t);
	void RenderText(Ref<Text> text, Transform t);
	void RenderTexture(Ref<Texture> texture, Transform t);
};

}