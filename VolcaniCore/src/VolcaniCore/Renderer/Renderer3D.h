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

	// Can draw meshes loaded from files or defined in the code, such as cubes
	static void DrawMesh(Ref<Mesh> mesh,	Transform t = { });
	static void DrawModel(Ref<Model> model, Transform t = { });

	static void DrawQuad(Ref<Quad> quad,		 Transform t = { });
	static void DrawQuad(Ref<Texture> texture,	 Transform t = { });
	static void DrawQuad(const glm::vec4& color, Transform t = { });

	static void DrawText(Ref<Text> text, Transform t = { });

private:
	static void Init();
	static void Close();

	static void Flush();

	friend class Renderer;
};

}