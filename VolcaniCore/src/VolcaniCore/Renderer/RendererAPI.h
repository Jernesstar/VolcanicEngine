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
struct DrawPass;
struct DrawCommand;
struct DrawCall;

class RendererAPI {
public:
	enum class Backend { OpenGL, Vulkan, DirectX };

	static const uint32_t MaxTextureSlots;

public:
	static void Create(RendererAPI::Backend backend);
	static void Shutdown();

	static Ref<RendererAPI> Get() { return s_Instance; }

public:
	RendererAPI(RendererAPI::Backend backend)
		: m_Backend(backend) { }
	virtual ~RendererAPI() = default;

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	virtual DrawBuffer* NewDrawBuffer(DrawBufferSpecification& specs,
									  void* data = nullptr) = 0;
	virtual DrawBuffer* GetDrawBuffer(DrawBufferSpecification& specs) = 0;
	virtual void SetBufferData(DrawBuffer* buffer, uint32_t bufferIndex,
							   void* data, uint64_t count) = 0;
	virtual void ReleaseBuffer(DrawBuffer* buffer) = 0;

	virtual void SubmitCommand(DrawCommand& command) = 0;

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
};

struct DrawCommand {
	DrawBuffer* BufferData;
	// Uniforms UniformData;
	Ref<ShaderPipeline> Pipeline;
	Ref<Framebuffer> Output;
	List<DrawCall> Calls;

	uint32_t ViewportWidth = 0;
	uint32_t ViewportHeight = 0;

	uint64_t VerticesIndex	= 0;
	uint64_t IndicesIndex	= 0;
	uint64_t InstancesIndex = 0;

	void AddIndices(Buffer<uint32_t> data) {
		RendererAPI::Get()
			->SetBufferData(BufferData, 0, data.Get(), data.GetCount());
		IndicesIndex += data.GetCount();
	}

	template<typename T>
	void AddVertices(Buffer<T> data) {
		RendererAPI::Get()
			->SetBufferData(BufferData, 1, data.Get(), data.GetCount());
		VerticesIndex += data.GetCount();
	}

	template<typename T>
	void AddInstances(Buffer<T> data) {
		RendererAPI::Get()
			->SetBufferData(BufferData, 2, data.Get(), data.GetCount());
		InstancesIndex += data.GetCount();
	}

	void ClearBuffers() {
		RendererAPI::Get()->SetBufferData(BufferData, 0, nullptr, 0);
		RendererAPI::Get()->SetBufferData(BufferData, 1, nullptr, 0);
		RendererAPI::Get()->SetBufferData(BufferData, 2, nullptr, 0);

		VerticesIndex  = 0;
		IndicesIndex   = 0;
		InstancesIndex = 0;
	}

	DrawCall& NewDrawCall() {
		return Calls.emplace_back(VerticesIndex, 0, IndicesIndex, 0);
	}
};

enum class DepthTestingMode { On, Off };
enum class BlendingMode { Off, Greatest, Additive };
enum class CullingMode { Off, Front, Back };

enum class PrimitiveType { Point, Line, Triangle, Cubemap };
enum class PartitionType { Single, Instanced, MultiDraw };

struct DrawCall {
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