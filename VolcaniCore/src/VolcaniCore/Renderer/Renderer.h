#pragma once

#include "Core/Defines.h"
#include "Core/Buffer.h"

#include "Object/Vertex.h"
#include "Object/Point.h"
#include "Object/Line.h"
#include "Object/Mesh.h"

#include "RenderPass.h"

namespace VolcaniCore {

enum class DrawType { Array, Indexed };
enum class DrawPartition { Single, Instanced, Multi };
enum class DrawPrimitive { Point, Line, Mesh };

struct DrawOptions {
	DrawType Type;
	DrawPartition Partition;
};

struct DrawCall {
	DrawType Type;
	DrawPartition Partition;
	DrawPrimitive Primitive;

	Buffer<Vertex> GeometryBuffer;
	Buffer<uint32_t> IndexBuffer;
	Buffer<glm::mat4> TransformBuffer;
};

using DrawOptionsMap = Map<DrawPrimitive, DrawOptions>;

struct DrawCommand {
	Ref<RenderPass> Pass;
	DrawOptionsMap OptionsMap;

	bool Clear = false;
	glm::ivec2 Size;

	void AddPoint(Ref<Point> point, const glm::mat4& transform);
	void AddLine(Ref<Line> line,	const glm::mat4& transform);
	void AddMesh(Ref<Mesh> mesh,	const glm::mat4& transform);

	Map<Ref<Point>, Buffer<glm::mat4>> Points;
	Map<Ref<Line>,  Buffer<glm::mat4>> Lines;
	Map<Ref<Mesh>,  Buffer<glm::mat4>> Meshes;
};

struct FrameDebugInfo {
	uint32_t DrawCalls;
	float FPS;
};

struct FrameData {
	List<DrawCommand> DrawCommands;
	FrameDebugInfo Info;

	void AddDrawCommand(DrawCommand command);
};

class Renderer {
public:
	static void StartPass(Ref<RenderPass> pass, const DrawOptionsMap& map = { });
	static void EndPass();

	static void Clear(const glm::vec4& color = glm::vec4(0.0f));
	static void Resize(uint32_t width, uint32_t height);

	static void BeginFrame();
	static void EndFrame();

	static Ref<RenderPass> GetPass();
	static DrawCommand& GetDrawCommand();
	static FrameDebugInfo& GetDebugInfo();

private:
	static void Init();
	static void Close();
	static FrameData& GetFrame();

	static void Flush(DrawCommand& command);
	static List<DrawCall> CreateDrawCalls(DrawCommand& command);

	friend class Application;
};

}