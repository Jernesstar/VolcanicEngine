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
	Uniforms PerUniforms;
	DrawOptionsMap OptionsMap;

	bool Clear = false;
	glm::ivec2 Size;

	Map<Ref<Point>, Buffer<glm::mat4>> Points;
	Map<Ref<Line>,  Buffer<glm::mat4>> Lines;
	Map<Ref<Mesh>,  Buffer<glm::mat4>> Meshes;

	void AddPoint(Ref<Point> point, const glm::mat4& transform);
	void AddLine(Ref<Line> line,	const glm::mat4& transform);
	void AddMesh(Ref<Mesh> mesh,	const glm::mat4& transform);

	Uniforms& GetUniforms() { return PerUniforms; }
};

struct FrameDebugInfo {
	uint32_t DrawCalls;
	float FPS;

	static const uint32_t MaxInstances;
	static const uint32_t MaxTriangles;
	static const uint32_t MaxVertices;
	static const uint32_t MaxIndices;
};

struct FrameData {
	List<DrawCommand> DrawCommands;
	FrameDebugInfo Info;

	void AddDrawCommand(DrawCommand& command);
};

class Renderer {
public:
	static void BeginFrame();
	static void EndFrame();

	static void StartPass(Ref<RenderPass> pass);
	static void EndPass();

	static void NewDrawCommand(const DrawOptionsMap& = { });

	static void Clear(const glm::vec4& color = glm::vec4(0.0f));
	static void Resize(uint32_t width, uint32_t height);

	static Ref<RenderPass> GetPass();
	static DrawCommand& GetDrawCommand();
	static FrameDebugInfo GetDebugInfo();

private:
	static void Init();
	static void Close();
	static FrameData& GetFrame();

	friend class Application;
};

}