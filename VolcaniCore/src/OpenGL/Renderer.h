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
	void Clear();

	void RenderMesh(Ref<Mesh> mesh);
	void RenderCubemap(Ref<Cubemap> cubemap);
	void RenderQuad(Ref<Quad> quad, Transform t);
	void RenderText(Ref<Text> text, Transform t);
	void RenderTexture(Ref<Texture> texture, Transform t);

	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);

private:
    VertexBuffer* s_CubemapBuffer;
    VertexArray* s_CubemapArray;
};

}