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
struct DrawUniforms;
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
		const void* data, uint64_t count, uint64_t offset = 0) = 0;
	virtual void ReleaseBuffer(DrawBuffer* buffer) = 0;

	virtual DrawCommand* NewDrawCommand(DrawBuffer* buffer = nullptr) = 0;

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

	uint64_t MaxIndexCount	  = 0;
	uint64_t MaxVertexCount	  = 0;
	uint64_t MaxInstanceCount = 0;
};

struct DrawBuffer {
	DrawBufferSpecification Specs;

	uint64_t IndicesCount	= 0;
	uint64_t VerticesCount	= 0;
	uint64_t InstancesCount = 0;

	void AddIndices(const Buffer<uint32_t>& data) {
		RendererAPI::Get()
		->SetBufferData(this, 0, data.Get(), data.GetCount(), IndicesCount);
	}

	template<typename T>
	void AddVertices(const Buffer<T>& data) {
		RendererAPI::Get()
		->SetBufferData(this, 1, data.Get(), data.GetCount(), VerticesCount);
	}

	template<typename T>
	void AddInstances(const Buffer<T>& data) {
		RendererAPI::Get()
		->SetBufferData(this, 2, data.Get(), data.GetCount(). InstancesCount);
	}

	void Clear() {
		RendererAPI::Get()->SetBufferData(this, 0, nullptr, 0);
		RendererAPI::Get()->SetBufferData(this, 1, nullptr, 0);
		RendererAPI::Get()->SetBufferData(this, 2, nullptr, 0);
	}
};

struct DrawPass {
	Ref<Framebuffer> Output;
};

struct TextureSlot {
	Ref<Texture> Sampler = nullptr;
	uint32_t Index = 0;
};

struct DrawUniforms {
	Map<std::string, int32_t> IntUniforms;
	Map<std::string, float> FloatUniforms;
	Map<std::string, TextureSlot> TextureUniforms;

	Map<std::string, glm::vec2> Vec2Uniforms;
	Map<std::string, glm::vec3> Vec3Uniforms;
	Map<std::string, glm::vec4> Vec4Uniforms;

	Map<std::string, glm::mat2> Mat2Uniforms;
	Map<std::string, glm::mat3> Mat3Uniforms;
	Map<std::string, glm::mat4> Mat4Uniforms;

	void SetInt(const std::string& name, int32_t data) {
		IntUniforms[name] = data;
	}
	void SetFloat(const std::string& name, float data) {
		FloatUniforms[name] = data;
	}
	void SetTexture(const std::string& name, TextureSlot data) {
		TextureUniforms[name] = data;
	}
	void SetVec2(const std::string& name, glm::vec2 data) {
		Vec2Uniforms[name] = data;
	}
	void SetVec3(const std::string& name, glm::vec3 data) {
		Vec3Uniforms[name] = data;
	}
	void SetVec4(const std::string& name, glm::vec4 data) {
		Vec4Uniforms[name] = data;
	}
	void SetMat2(const std::string& name, glm::mat2 data) {
		Mat2Uniforms[name] = data;
	}
	void SetMat3(const std::string& name, glm::mat3 data) {
		Mat3Uniforms[name] = data;
	}
	void SetMat4(const std::string& name, glm::mat4 data) {
		Mat4Uniforms[name] = data;
	}
};

struct DrawCommand {
	DrawBuffer* BufferData;
	DrawUniforms UniformData;
	Ref<ShaderPipeline> Pipeline;
	Ref<Framebuffer> Image;
	List<DrawCall> Calls;

	bool Clear = true;
	uint32_t ViewportWidth = 0;
	uint32_t ViewportHeight = 0;

	uint64_t IndicesIndex	= 0;
	uint64_t VerticesIndex	= 0;
	uint64_t InstancesIndex = 0;

	DrawCommand(DrawBuffer* buffer)
		: BufferData(buffer)
	{
		if(!buffer)
			return;

		IndicesIndex = BufferData->IndicesCount;
		VerticesIndex = BufferData->VerticesCount;
		InstancesIndex = BufferData->InstancesCount;
	}

	void AddIndices(const Buffer<uint32_t>& data) {
		BufferData->AddIndices(data);
		IndicesIndex += data.GetCount();
	}

	template<typename T>
	void AddVertices(const Buffer<T>& data) {
		BufferData->AddVertices(data);
		VerticesIndex += data.GetCount();
	}

	template<typename T>
	void AddInstances(const Buffer<T>& data) {
		BufferData->AddInstances(data);
		InstancesIndex += data.GetCount();
	}

	DrawCall& NewDrawCall() {
		return Calls.emplace_back(IndicesIndex, 0, VerticesIndex, 0,
								  InstancesIndex, 0);
	}
};

enum class DepthTestingMode { On, Off };
enum class BlendingMode { Off, Greatest, Additive };
enum class CullingMode { Off, Front, Back };

enum class PrimitiveType { Point, Line, Triangle, Cubemap };
enum class PartitionType { Single, Instanced, MultiDraw };

struct DrawCall {
	uint64_t IndexStart	   = 0;
	uint64_t IndexCount	   = 0;
	uint64_t VertexStart   = 0;
	uint64_t VertexCount   = 0;
	uint64_t InstanceStart = 0;
	uint64_t InstanceCount = 0;

	PartitionType Partition = PartitionType::Instanced;
	PrimitiveType Primitive = PrimitiveType::Triangle;

	DepthTestingMode DepthTest = DepthTestingMode::On;
	BlendingMode Blending	   = BlendingMode::Greatest;
	CullingMode Culling		   = CullingMode::Back;
};

}