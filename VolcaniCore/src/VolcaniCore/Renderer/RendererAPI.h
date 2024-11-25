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

	List<DrawCall> Calls;
};

struct DrawCall {
	Buffer<void> Vertices;
	Buffer<uint32_t> Indices;
	Buffer<void> InstanceData;

	struct Modes {
		enum class DepthTestingMode {
			On,
			Off
		} DepthTest = DepthTestingMode::On;

		enum class BlendingModd {
			Off,
			Greatest,
			Additive
		} Blending = BlendingModd::Greatest;

		enum class CullingMode {
			Off,
			Front,
			Back
		} Cull = CullingMode::Back;
	} const ModeOptions;

	class Types {
		enum class PrimitiveType {
			Point,
			Line,
			Triangle,
			Cubemap
		} Primitive = PrimitiveType::Triangle;
		enum class PartitionType {
			Single,
			Instanced,
			MultiDraw
		} Partition = PartitionType::Instanced;
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

	virtual DrawCommand CreateDrawCommand(
		BufferLayout vertexLayout, BufferLayout instanceLayout = { }) = 0;

	virtual void SubmitDrawCommand(DrawCommand& command) = 0;

protected:
	const RendererAPI::Backend m_Backend;

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}