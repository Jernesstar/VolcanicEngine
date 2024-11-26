#pragma once

#include "Object/Cubemap.h"
#include "Object/Framebuffer.h"
#include "Object/Shader.h"

#include "Core/Buffer.h"

#include "Transform.h"
#include "BufferLayout.h"

namespace VolcaniCore {

struct DrawBufferSpecification;
struct DrawBuffer;
struct DrawCall;

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

	virtual void StartFrame() = 0;
	virtual void EndFrame()   = 0;

	virtual DrawBuffer* NewDrawBuffer(DrawBufferSpecification& specs,
									  void* data = nullptr) = 0;
	virtual DrawBuffer* GetDrawBuffer(DrawBufferSpecification& specs) = 0;

	virtual void Submit(DrawCall& call) = 0;

	virtual void ReleaseBuffer(DrawBuffer* buffer) = 0;

	RendererAPI::Backend GetBackend() const { return m_Backend; }

protected:
	const RendererAPI::Backend m_Backend;

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

struct DrawBufferSpecification {
	BufferLayout VertexLayout;
	BufferLayout InstanceLayout = { };

	uint64_t MaxVertexCount	  = 0;
	uint64_t MaxIndexCount	  = 0;
	uint64_t MaxInstanceCount = 0;
};

struct DrawBuffer {
	DrawBufferSpecification Specs;

	Buffer<void> Vertices;
	Buffer<uint32_t> Indices;
	Buffer<void> Instances;
};

enum class DepthTestingMode { On, Off };
enum class BlendingMode { Off, Greatest, Additive };
enum class CullingMode { Off, Front, Back };

enum class PrimitiveType { Point, Line, Triangle, Cubemap };
enum class PartitionType { Single, Instanced, MultiDraw };

struct DrawCall {
	DrawBuffer* Buffer;
	Ref<ShaderPipeline> Pipeline;
	// TODO(Change): Perhaps?
	// Uniforms Uniforms;

	uint64_t VertexStart   = 0;
	uint64_t VertexCount   = 0;
	uint64_t IndexCount	   = 0;
	uint64_t InstanceCount = 0;

	PartitionType Partition = PartitionType::Instanced;
	PrimitiveType Primitive = PrimitiveType::Triangle;

	DepthTestingMode DepthTest = DepthTestingMode::On;
	BlendingMode Blending	   = BlendingMode::Greatest;
	CullingMode Culling		   = CullingMode::Back;
};

}