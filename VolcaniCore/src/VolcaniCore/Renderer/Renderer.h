#pragma once

#include "Renderer/RenderPass.h"

namespace VolcaniCore {


struct FrameData {

};

class Renderer {
public:
	static void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f });
	static void Resize(uint32_t width, uint32_t height);

	static void Begin();
	static void End();

	static void StartPass(Ref<RenderPass> pass);
	static void EndPass(Ref<RenderPass> pass);

	static FrameData& GetFrameData() { return s_FrameData; }

private:
	inline static FrameData s_FrameData;

	static void Init();
	static void Close();

	friend class Application;
};


}