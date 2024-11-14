#pragma once

#include "Object/Cubemap.h"
#include "Object/Framebuffer.h"

#include "Core/Buffer.h"

#include "Transform.h"
#include "BufferLayout.h"

namespace VolcaniCore {

struct DrawCommand {
	Buffer<void> Vertices;
	Buffer<uint32_t> Indices;
	Buffer<void> InstanceData;

	BufferLayout VertexLayout;
	BufferLayout InstanceLayout;
	Ref<ShaderPipeline> Pipeline;
};

struct DrawCall {
	class Modes {
	public:
		enum class DepthTestingMode {
			On,
			Off
		} DepthTest = DepthTestingMode::On;

		enum class BlendingMode {
			Off,
			Greatest,
			Additive
		} Blending = BlendingMode::Greatest;

		enum class CullingMode {
			Off,
			Front,
			Back
		} Cull = CullingMode::Back;
	} const ModeOptions;

	class Type {
		enum class Primitive { Point, Line, Triangle, Cubemap };
		enum class Partition { Single, Instanced, MultiDraw };
	} const TypeOptions;
};

class RendererAPI {
public:
	enum class Backend { OpenGL, Vulkan, DirectX };

public:
	static void Create(RendererAPI::Backend backend);
	static void Shutdown();

	static Ref<RendererAPI> Get() { return s_Instance; }

public:
	RendererAPI(RendererAPI::Backend backend)
		: m_Backend(backend) { }
	virtual ~RendererAPI() = default;

	RendererAPI::Backend GetBackend() const { return m_Backend; }

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	virtual DrawCommand CreateDrawCommand(BufferLayout vertexLayout,
										  BufferLayout instanceLayout = { }) = 0;
	virtual void SubmitDrawCommand(DrawCommand& command) = 0;
	virtual void SubmitDrawCall(DrawCall& call) = 0;

	virtual void SetOptions(const DrawCall::Option& options)	 = 0;
	virtual void Clear(const glm::vec4& color = glm::vec4(0.0f)) = 0;
	virtual void Resize(uint32_t width, uint32_t height)		 = 0;

protected:
	const RendererAPI::Backend m_Backend;

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}