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
	static Ref<RenderPass> GetPass();

	static DrawCommand* GetCommand();
	static DrawCommand* NewCommand();
	static void EndCommand();

	static void Clear();
	static void Resize(uint32_t width, uint32_t height);

	static void Flush();

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