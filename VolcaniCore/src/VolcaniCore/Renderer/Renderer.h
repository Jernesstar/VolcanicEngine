#pragma once

#include <vector>
#include <unordered_map>

#include "Renderer/RenderPass.h"

#include "Object/Quad.h"
#include "Object/Mesh.h"

namespace VolcaniCore {

// TODO: Rename ?
// struct RenderPassData {
struct FrameData {
	std::vector<Ref<Quad>> Quads3D;
	std::vector<Ref<Quad>> Quads2D;

	std::unordered_map<Ref<Mesh>, uint32_t> Meshes;
};

class Renderer {
public:
	static void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f });
	static void Resize(uint32_t width, uint32_t height);

	static void BeginFrame();
	static void EndFrame();

	static void StartPass(Ref<RenderPass> pass);
	static void EndPass();

	static FrameData& GetFrameData() { return s_FrameData; }

private:
	inline static FrameData s_FrameData;

	static void Init();
	static void Close();

	friend class Application;
};


}