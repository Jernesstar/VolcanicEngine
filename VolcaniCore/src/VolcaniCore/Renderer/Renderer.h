#pragma once

#include "Core/Buffer.h"

#include "RenderPass.h"

namespace VolcaniCore {

struct DrawCommand {
	Ref<RenderPass> Pass;
	bool ShouldClearScreen = false;

	std::unordered_map<Ref<Mesh>, Buffer<glm::mat4>> MeshTransforms;
	std::unordered_map<Point, Buffer<glm::mat4>> PointTransforms;
	std::unordered_map<Line, Buffer<glm::mat4>> LineTransforms;
};

struct FrameData {
	std::vector<DrawCommand> DrawCommands;

	void AddDrawCommand(const DrawCommand& command) {
		DrawCommands.push_back(command);
	}

	std::vector<DrawCommand>::const_iterator begin() const {
		return DrawCommands.begin();
	}
	std::vector<DrawCommand>::const_iterator end() const {
		return DrawCommands.end();
	}
}

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