#pragma once

#include "Graphics/Camera.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Transform.h"

#include "Graphics/Cubemap.h"
#include "Graphics/Point.h"
#include "Graphics/Line.h"
#include "Graphics/Quad.h"
#include "Graphics/Mesh.h"
#include "Graphics/Model.h"
#include "Graphics/Texture.h"
#include "Graphics/Text.h"

namespace VolcaniCore {

class Renderer3D {
public:
	static void StartFrame();
	static void EndFrame();

	static void Begin(Ref<Camera> camera);
	static void End();

	static void DrawSkybox(Ref<Cubemap> cubemap);

	static void DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr);
	static void DrawMesh(Ref<Mesh> mesh, const Transform& t = { }) {
		DrawMesh(mesh, t.GetTransform());
	}

	static void DrawModel(Ref<Model> model, const glm::mat4& tr);
	static void DrawModel(Ref<Model> model, const Transform& t = { }) {
		DrawModel(model, t.GetTransform());
	}

	static void DrawQuad(Ref<Quad> quad, const glm::mat4& tr);
	static void DrawQuad(Ref<Quad> quad, const Transform& t = { }) {
		DrawQuad(quad, t.GetTransform());
	}

	static void DrawQuad(Ref<Texture> texture, const glm::mat4& tr);
	static void DrawQuad(Ref<Texture> texture, const Transform& t = { }) {
		DrawQuad(texture, t.GetTransform());
	}

	static void DrawQuad(const glm::vec4& color, const glm::mat4& tr);
	static void DrawQuad(const glm::vec4& color, const Transform& t = { }) {
		DrawQuad(color, t.GetTransform());
	}

	static void DrawLine(const Line& line, const glm::mat4& tr);
	static void DrawLine(const Line& line, const Transform& t = { }) {
		DrawLine(line, t.GetTransform());
	}

	static void DrawPoint(const Point& point, const glm::mat4& tr);
	static void DrawPoint(const Point& point, const Transform& t = { }) {
		DrawPoint(point, t.GetTransform());
	}

	static void DrawText(Ref<Text> text, const glm::mat4& tr);
	static void DrawText(Ref<Text> text, const Transform& t = { }) {
		DrawText(text, t.GetTransform());
	}

private:
	static void Init();
	static void Close();

	friend class Renderer;
};

}