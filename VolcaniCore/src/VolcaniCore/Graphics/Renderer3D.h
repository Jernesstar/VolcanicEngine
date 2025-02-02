#pragma once

#include "Graphics/RendererAPI.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Transform.h"
#include "Graphics/Camera.h"

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
	static DrawBuffer* GetMeshBuffer();
	static DrawBuffer* GetCubemapBuffer();

	static void Begin(Ref<Camera> camera);
	static void End();

	static void DrawSkybox(Ref<Cubemap> cubemap);

	static void DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawMesh(Ref<Mesh> mesh, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawMesh(mesh, t.GetTransform(), command);
	}

	static void DrawModel(Ref<Model> model, const glm::mat4& tr,
						  DrawCommand* command = nullptr);
	static void DrawModel(Ref<Model> model, const Transform& t = { },
						  DrawCommand* command = nullptr)
	{
		DrawModel(model, t.GetTransform(), command);
	}

	static void DrawQuad(Ref<Quad> quad, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawQuad(Ref<Quad> quad, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawQuad(quad, t.GetTransform(), command);
	}

	static void DrawQuad(Ref<Texture> texture, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawQuad(Ref<Texture> texture, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawQuad(texture, t.GetTransform(), command);
	}

	static void DrawQuad(const glm::vec4& color, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawQuad(const glm::vec4& color, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawQuad(color, t.GetTransform(), command);
	}

	static void DrawLine(const Line& line, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawLine(const Line& line, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawLine(line, t.GetTransform(), command);
	}

	static void DrawPoint(const Point& point, const glm::mat4& tr,
						  DrawCommand* command = nullptr);
	static void DrawPoint(const Point& point, const Transform& t = { },
						  DrawCommand* command = nullptr)
	{
		DrawPoint(point, t.GetTransform(), command);
	}

	static void DrawText(Ref<Text> text, const glm::mat4& tr,
						 DrawCommand* command = nullptr);
	static void DrawText(Ref<Text> text, const Transform& t = { },
						 DrawCommand* command = nullptr)
	{
		DrawText(text, t.GetTransform(), command);
	}

private:
	static void Init();
	static void Close();

	friend class Renderer;
};

}