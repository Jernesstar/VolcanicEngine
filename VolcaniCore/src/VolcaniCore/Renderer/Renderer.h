#pragma once

#include <unordered_map>

#include "Core/Buffer.h"

#include "Object/Point.h"
#include "Object/Line.h"
#include "Object/Mesh.h"

#include "RenderPass.h"

namespace VolcaniCore {

template<typename TData>
using List = std::vector<TData>;

template<typename TData>
using PrimitiveData = std::unordered_map<Ref<TData>, Buffer<glm::mat4>>;

// template<typename TData>
// struct PrimitiveData;

struct DrawCommand {
	Ref<RenderPass> Pass;
	bool ShouldClearScreen = false;

	void AddPoint(Ref<Point> point, const glm::mat4& transform);
	void AddLine(Ref<Line> line, const glm::mat4& transform);
	void AddMesh(Ref<Mesh> mesh, const glm::mat4& transform);

private:
	PrimitiveData<Point> m_Points;
	PrimitiveData<Line> m_Lines;
	PrimitiveData<Mesh> m_Meshes;
};

enum class DrawType { Array, Indexed };
enum class DrawOption { Single, Instanced, Multi };
enum class DrawPrimitive { Point, Line, Mesh };

struct DrawCall {
	DrawType Type;
	DrawOption Options;
	DrawPrimitive Primitive;

};

struct FrameDebugInfo {
	uint32_t DrawCalls;
	float FPS;
};

struct FrameData {
	List<DrawCommand> DrawCommands;
	FrameDebugInfo Info;

	void AddDrawCommand(DrawCommand& command);
};

class Renderer {
public:
	static void Clear(const glm::vec4& color = glm::vec4(0.0f));
	static void Resize(uint32_t width, uint32_t height);

	static void StartPass(Ref<RenderPass> pass);
	static void EndPass();

	static void BeginFrame();
	static void EndFrame();

	static Ref<RenderPass> GetPass();
	static DrawCommand& GetDrawCommand();

private:
	static void Init();
	static void Close();

	static void Flush(DrawCommand& command);
	static List<DrawCall> CreateDrawCalles(DrawCommand& command);

	friend class Application;
};

}