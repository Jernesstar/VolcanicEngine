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

	static void DrawMesh(Ref<Mesh> mesh, const Transform& t = { });
	static void DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr);

	static void DrawModel(Ref<Model> model, const Transform& t = { });
	static void DrawModel(Ref<Model> model, const glm::mat4& tr);

	static void DrawQuad(Ref<Quad> quad, const Transform& t = { });
	static void DrawQuad(Ref<Quad> quad, const glm::mat4& tr);

	static void DrawQuad(Ref<Texture> texture, const Transform& t = { });
	static void DrawQuad(Ref<Texture> texture, const glm::mat4& tr);

	static void DrawQuad(const glm::vec4& color, const Transform& t = { });
	static void DrawQuad(const glm::vec4& color, const glm::mat4& tr);

	static void DrawText(Ref<Text> text, const Transform& t = { });
	static void DrawText(Ref<Text> text, const glm::mat4& tr);

private:
	static void Init();
	static void Close();
	static void Flush();

	friend class Renderer;
};

}