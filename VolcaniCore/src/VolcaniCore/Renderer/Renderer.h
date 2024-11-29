#pragma once

#include "Core/Defines.h"
#include "Core/Buffer.h"

#include "Object/Vertex.h"
#include "Object/Point.h"
#include "Object/Line.h"
#include "Object/Mesh.h"

#include "RendererAPI.h"
#include "RenderPass.h"

namespace VolcaniCore {

struct FrameDebugInfo {
	float FPS;

	uint32_t DrawCalls = 0;
	uint32_t Indices   = 0;
	uint32_t Vertices  = 0;
	uint32_t Instances = 0;
};

struct FrameData {
	// List<DrawCall> DrawCalls;
	FrameDebugInfo Info;
};

class Renderer {
public:
	static const uint64_t MaxTriangles;
	static const uint64_t MaxIndices;
	static const uint64_t MaxVertices;
	static const uint64_t MaxInstances;

public:
	static void StartPass(Ref<RenderPass> pass);
	static void EndPass();

	static void NewDrawCommand();

	static void Clear(const glm::vec4& color = glm::vec4(0.0f)) { }
	static void Resize(uint32_t width, uint32_t height) { }

	static Ref<RenderPass> GetPass();
	static FrameDebugInfo GetDebugInfo();

private:
	static void Init();
	static void Close();

	static void BeginFrame();
	static void EndFrame();

	static FrameData& GetFrame();

	friend class Application;
};

}