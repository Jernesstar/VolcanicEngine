#pragma once

#include "Object/Cubemap.h"
#include "Object/Quad.h"
#include "Object/Text.h"
#include "Object/Texture.h"
#include "Object/Shader.h"

#include "Object/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/Transform.h"

namespace VolcaniCore {

class Renderer3D {
public:
	static void Begin(Ref<Camera> camera);
	static void End();

	static void DrawCubemap(Ref<Cubemap> cubemap);

	static void DrawMesh(Ref<Mesh> mesh,	const glm::mat4& transform = { });
	static void DrawModel(Ref<Model> model, const glm::mat4& transform = { });

	static void DrawQuad(Ref<Quad> quad, const glm::mat4& transform = { });
	static void DrawQuad(Ref<Texture> texture,
						 const glm::mat4& transform = { });
	static void DrawQuad(const glm::vec4& color,
						 const glm::mat4& transform = { });

	static void DrawText(Ref<Text> text, const glm::mat4& transform = { });

private:
	static void Init();
	static void Close();
	static void Flush();

	friend class Renderer;
};

}