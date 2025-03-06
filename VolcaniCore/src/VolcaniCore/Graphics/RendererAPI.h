#pragma once

#include "Core/Buffer.h"
#include "Core/List.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Cubemap.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/Framebuffer.h"

#include "BufferLayout.h"

namespace VolcaniCore {

struct DrawBufferSpecification;
struct DrawBuffer;
struct DrawUniforms;
struct DrawPass;
struct DrawCommand;
struct DrawCall;

struct DebugInfo {
	uint64_t DrawCallCount = 0;
	uint64_t IndexCount    = 0;
	uint64_t VertexCount   = 0;
	uint64_t InstanceCount = 0;
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

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;
	virtual DebugInfo GetDebugInfo() = 0;

	virtual DrawBuffer* NewDrawBuffer(DrawBufferSpecification& specs,
									  void* data = nullptr) = 0;
	virtual DrawBuffer* GetDrawBuffer(DrawBufferSpecification& specs) = 0;
	virtual void SetBufferData(DrawBuffer* buffer, uint8_t bufferIndex,
		const void* data, uint64_t count, uint64_t offset = 0) = 0;
	virtual void ReleaseBuffer(DrawBuffer* buffer) = 0;

	virtual DrawPass* NewDrawPass(
		DrawBuffer* buffer, Ref<ShaderPipeline> pipeline,
		Ref<Framebuffer> output = nullptr) = 0;

	virtual DrawCommand* NewDrawCommand(DrawPass* pass) = 0;

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

enum DrawBufferIndex : uint8_t { Indices, Vertices, Instances };

struct DrawBuffer {
	DrawBufferSpecification Specs;

	uint64_t IndicesCount	= 0;
	uint64_t VerticesCount	= 0;
	uint64_t InstancesCount = 0;

	void AddIndices(const Buffer<uint32_t>& data) {
		RendererAPI::Get()
		->SetBufferData(this, DrawBufferIndex::Indices,
						data.Get(), data.GetCount(), IndicesCount);
	}

	template<typename T>
	void AddVertices(const Buffer<T>& data) {
		RendererAPI::Get()
		->SetBufferData(this, DrawBufferIndex::Vertices,
						data.Get(), data.GetCount(), VerticesCount);
	}

	template<typename T>
	void AddInstances(const Buffer<T>& data) {
		RendererAPI::Get()
		->SetBufferData(this, DrawBufferIndex::Instances,
						data.Get(), data.GetCount(), InstancesCount);
	}

	void Clear(DrawBufferIndex idx) {
		RendererAPI::Get()->SetBufferData(this, idx, nullptr, 0);
	}

	void Clear() {
		Clear(DrawBufferIndex::Indices);
		Clear(DrawBufferIndex::Vertices);
		Clear(DrawBufferIndex::Instances);
	}
};

struct DrawPass {
	DrawBuffer* BufferData;
	Ref<ShaderPipeline> Pipeline;
	Ref<Framebuffer> Output;
};

struct TextureSlot {
	Ref<Texture> Sampler = nullptr;
	uint32_t Index = 0;
};

struct CubemapSlot {
	Ref<Cubemap> Sampler = nullptr;
	uint32_t Index = 0;
};

struct UniformSlot {
	Ref<UniformBuffer> Buffer = nullptr;
	std::string Name = "";
	uint32_t Binding = 0;
};

struct DrawUniforms {
	Map<std::string, int32_t> IntUniforms;
	Map<std::string, float> FloatUniforms;
	Map<std::string, TextureSlot> TextureUniforms;
	Map<std::string, CubemapSlot> CubemapUniforms;

	Map<std::string, glm::vec2> Vec2Uniforms;
	Map<std::string, glm::vec3> Vec3Uniforms;
	Map<std::string, glm::vec4> Vec4Uniforms;

	Map<std::string, glm::mat2> Mat2Uniforms;
	Map<std::string, glm::mat3> Mat3Uniforms;
	Map<std::string, glm::mat4> Mat4Uniforms;

	List<UniformSlot> UniformBuffers;

	operator bool () const {
		return UniformBuffers || IntUniforms.size() || FloatUniforms.size()
		|| TextureUniforms.size() || CubemapUniforms.size()
		|| Vec2Uniforms.size() || Vec3Uniforms.size() || Vec4Uniforms.size()
		|| Mat2Uniforms.size() || Mat3Uniforms.size() || Mat4Uniforms.size();
	}

	void SetInput(const std::string& name, int32_t data) {
		IntUniforms[name] = data;
	}
	void SetInput(const std::string& name, float data) {
		FloatUniforms[name] = data;
	}
	void SetInput(const std::string& name, const TextureSlot& data) {
		TextureUniforms[name] = data;
	}
	void SetInput(const std::string& name, const CubemapSlot& data) {
		CubemapUniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::vec2& data) {
		Vec2Uniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::vec3& data) {
		Vec3Uniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::vec4& data) {
		Vec4Uniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::mat2& data) {
		Mat2Uniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::mat3& data) {
		Mat3Uniforms[name] = data;
	}
	void SetInput(const std::string& name, const glm::mat4& data) {
		Mat4Uniforms[name] = data;
	}
	void SetInput(const UniformSlot& data) {
		UniformBuffers.Add(data);
	}
};

struct DrawCommand {
	DrawPass* Pass;

	DrawUniforms UniformData;
	List<Pair<AttachmentTarget, uint32_t>> Outputs;
	List<DrawCall> Calls;

	bool Clear = false;
	uint32_t ViewportWidth = 0;
	uint32_t ViewportHeight = 0;

	uint64_t IndicesIndex	= 0;
	uint64_t VerticesIndex	= 0;
	uint64_t InstancesIndex = 0;

	uint64_t IndicesCount	= 0;
	uint64_t VerticesCount	= 0;
	uint64_t InstancesCount = 0;

	void AddIndices(const Buffer<uint32_t>& data) {
		Pass->BufferData->AddIndices(data);
		IndicesCount += data.GetCount();
	}

	template<typename T>
	void AddVertices(const Buffer<T>& data) {
		Pass->BufferData->AddVertices(data);
		VerticesCount += data.GetCount();
	}

	template<typename T>
	void AddInstances(const Buffer<T>& data) {
		Pass->BufferData->AddInstances(data);
		InstancesCount += data.GetCount();
	}

	void AddInstance(const void* data) {
		RendererAPI::Get()
		->SetBufferData(Pass->BufferData, DrawBufferIndex::Instances, data, 1,
						InstancesCount++);
	}

	DrawCall& NewDrawCall() {
		return Calls.Emplace(0U, IndicesCount, 0U, VerticesCount, 0U, 0U);
	}
};

enum class DepthTestingMode { On, Off };
enum class BlendingMode { Off, Greatest, Additive };
enum class CullingMode { Off, Front, Back };

enum class PrimitiveType { Point, Line, Triangle, Cubemap };
enum class PartitionType { Single, Instanced, MultiDraw };

struct DrawCall {
	uint64_t IndexStart	   = 0; // Relative
	uint64_t IndexCount	   = 0;
	uint64_t VertexStart   = 0; // Relative
	uint64_t VertexCount   = 0;
	uint64_t InstanceStart = 0; // Absolute
	uint64_t InstanceCount = 0;

	PartitionType Partition = PartitionType::Instanced;
	PrimitiveType Primitive = PrimitiveType::Triangle;

	DepthTestingMode DepthTest = DepthTestingMode::On;
	BlendingMode Blending	   = BlendingMode::Greatest;
	CullingMode Culling		   = CullingMode::Back;
};

}