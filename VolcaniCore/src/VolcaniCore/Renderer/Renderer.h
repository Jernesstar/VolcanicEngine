#pragma once

#include <unordered_map>

#include "Core/Buffer.h"

#include "Object/Point.h"
#include "Object/Line.h"
#include "Object/Mesh.h"

#include "RenderPass.h"

namespace VolcaniCore {

struct DrawCommand {
	Ref<RenderPass> Pass;
	bool ShouldClearScreen = false;

	std::unordered_map<Ref<Mesh>, Buffer<glm::mat4>> MeshTransforms;

	// Turn this into two lists, one a HashSet, one not
	// std::unordered_map<Point, Buffer<glm::mat4>> PointTransforms;
	// std::unordered_map<Line, Buffer<glm::mat4>> LineTransforms;
};

struct FrameDebugInfo {
	uint32_t DrawCalls;
	float FPS;
};

struct FrameData {
	std::vector<DrawCommand> DrawCommands;

	FrameDebugInfo Info;
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

	friend class Application;
};

}