#pragma once

#include <vector>
#include <unordered_map>

#include "Renderer/RenderPass.h"

#include "Object/Quad.h"
#include "Object/Mesh.h"

namespace VolcaniCore {

class Renderer {
public:
	static void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f });
	static void Resize(uint32_t width, uint32_t height);

	static void BeginFrame();
	static void EndFrame();

	static void StartPass(Ref<RenderPass> pass);
	static void EndPass();

	static Ref<RenderPass> GetPass();

private:
	static void Init();
	static void Close();

	friend class Application;
};

}