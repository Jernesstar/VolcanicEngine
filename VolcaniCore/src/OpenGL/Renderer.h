#pragma once

#include "Renderer/APIRenderer.h"
#include "Renderer/Mesh.h"

#include "VertexArray.h"
#include "VertexBuffer.h"

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::APIRenderer {
public:
	Renderer();
	~Renderer();

	void Init();
	void Close();
	void ClearScreen();

	void RenderMesh(Ref<Mesh> mesh);
	void RenderCubemap(Ref<Cubemap> cubemap);
	void RenderQuad(Ref<Quad> quad, Transform t);
	void RenderText(Ref<Text> text, Transform t);
	void RenderTexture(Ref<Texture> texture, Transform t);

private:
    VertexBuffer* s_CubemapBuffer;
    VertexArray* s_CubemapArray;
};

}