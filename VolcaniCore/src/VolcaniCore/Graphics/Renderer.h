#pragma once

#include "Core/Defines.h"

#include "RenderPass.h"

namespace VolcaniCore {

struct FrameDebugInfo {
	float FPS;

	uint64_t DrawCalls = 0;
	uint64_t Indices   = 0;
	uint64_t Vertices  = 0;
	uint64_t Instances = 0;
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
	static void StartPass(Ref<RenderPass> pass, bool pushCommand = true);
	static void EndPass();
	static Ref<RenderPass> GetPass();

	static DrawCommand* PushCommand();
	static void PopCommand();
	static DrawCommand* GetCommand();
	static DrawCommand* NewCommand(bool usePrevious = false);

	static void Clear();
	static void Resize(uint32_t width, uint32_t height);

	static void PushOptions();
	static void PopOptions(uint32_t count = 1);

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