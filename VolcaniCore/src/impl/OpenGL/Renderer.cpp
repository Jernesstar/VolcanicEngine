#include "Renderer.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <Core/Application.h>
#include <Core/Assert.h>
#include <Event/Events.h>

#include <Graphics/Renderer.h>
#include <Graphics/ShaderLibrary.h>

#include "Defines.h"

#include "Shader.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "Framebuffer.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"

#include "VertexArray.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

static void Clear();
static void Resize(uint32_t width, uint32_t height);

static void FlushCommand(DrawCommand&);
static void FlushCall(DrawCommand&, DrawCall&);

static void SetUniforms(DrawCommand& command);
static void SetOptions(DrawCommand& call);

struct BackendBuffer {
	Ref<VertexArray> Array;
	Buffer<uint32_t> Indices;
	Buffer<void> Vertices;
	Buffer<void> Instances;
};

struct RendererData {
	Map<DrawBuffer*, BackendBuffer> Arrays;
	List<DrawCommand> Commands;
	List<DrawPass> Passes;
	DrawPass* LastPass = nullptr;
};

static RendererData s_Data;
static DebugInfo s_Info;

Renderer::Renderer()
	: RendererAPI(RendererAPI::Backend::OpenGL)
{
	int success = gladLoadGL();
	VOLCANICORE_ASSERT(success, "Glad could not load OpenGL");
	VOLCANICORE_LOG_INFO("Successfully loaded OpenGL version %s",
		glGetString(GL_VERSION));
}

void Renderer::Init() {
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Smooth cubemap edges
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	s_Data.Commands.Allocate(100);
	s_Data.Passes.Allocate(5);
}

void Renderer::Close() {
	s_Data.Commands.Clear();
	s_Data.Passes.Clear();
	s_Data.Arrays.clear();
}

void Renderer::StartFrame() {
	s_Info.DrawCallCount = 0;
	s_Info.IndexCount    = 0;
	s_Info.VertexCount   = 0;
	s_Info.InstanceCount = 0;
}

void Renderer::EndFrame() {
	if(!s_Data.Commands)
		return;

	for(auto& [buffer, backend] : s_Data.Arrays) {
		if(backend.Indices.GetCount() && buffer->Specs.MaxIndexCount)
			backend.Array->GetIndexBuffer()->SetData(backend.Indices);
		if(backend.Vertices.GetCount() && buffer->Specs.MaxVertexCount)
			backend.Array->GetVertexBuffer(0)->SetData(backend.Vertices);
		if(backend.Instances.GetCount() && buffer->Specs.MaxInstanceCount) {
			uint32_t idx = buffer->Specs.MaxVertexCount != 0;
			backend.Array->GetVertexBuffer(idx)->SetData(backend.Instances);
		}
	}

	for(auto& command : s_Data.Commands)
		FlushCommand(command);

	s_Data.Passes.Clear();
	s_Data.Commands.Clear();
}

DebugInfo Renderer::GetDebugInfo() {
	return s_Info;
}

DrawBuffer* Renderer::NewDrawBuffer(DrawBufferSpecification& specs, void* data)
{
	auto* buffer = new DrawBuffer{ specs };
	auto array = CreateRef<VertexArray>();
	s_Data.Arrays.emplace(std::make_pair(buffer, array));

	if(data) {
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(
				specs.VertexLayout, specs.MaxVertexCount, data));
		buffer->Specs.MaxVertexCount = 0;

		return buffer;
	}

	if(specs.MaxIndexCount) {
		array->SetIndexBuffer(
			CreateRef<IndexBuffer>(specs.MaxIndexCount, true));
		s_Data.Arrays[buffer].Indices = Buffer<uint32_t>(specs.MaxIndexCount);
	}
	if(specs.MaxVertexCount) {
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(specs.VertexLayout, specs.MaxVertexCount));
		s_Data.Arrays[buffer].Vertices
			= Buffer<void>(specs.VertexLayout.Stride, specs.MaxVertexCount);
	}
	if(specs.MaxInstanceCount) {
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(
				specs.InstanceLayout, specs.MaxInstanceCount));
		s_Data.Arrays[buffer].Instances
			= Buffer<void>(specs.InstanceLayout.Stride, specs.MaxInstanceCount);
	}

	return buffer;
}

DrawBuffer* Renderer::GetDrawBuffer(DrawBufferSpecification& specs) {
	for(auto& [buffer, backendBuffer] : s_Data.Arrays)
		if(buffer->Specs.VertexLayout == specs.VertexLayout
		&& buffer->Specs.InstanceLayout == specs.InstanceLayout)
			return buffer;

	return NewDrawBuffer(specs);
}

void Renderer::SetBufferData(DrawBuffer* buffer, uint8_t bufferIndex,
							 const void* data, uint64_t count, uint64_t offset)
{
	auto& backendBuffer = s_Data.Arrays[buffer];
	if(bufferIndex == 0)
		if(count == 0)
			backendBuffer.Indices.Clear();
		else
			backendBuffer.Indices.Set(data, count, offset);
	else if(bufferIndex == 1)
		if(count == 0)
			backendBuffer.Vertices.Clear();
		else
			backendBuffer.Vertices.Set(data, count, offset);
	else if(bufferIndex == 2)
		if(count == 0)
			backendBuffer.Instances.Clear();
		else
			backendBuffer.Instances.Set(data, count, offset);

	buffer->IndicesCount = backendBuffer.Indices.GetCount();
	buffer->VerticesCount = backendBuffer.Vertices.GetCount();
	buffer->InstancesCount = backendBuffer.Instances.GetCount();
}

void Renderer::ReleaseBuffer(DrawBuffer* buffer) {
	s_Data.Arrays.erase(buffer);
	delete buffer;
}

DrawPass* Renderer::NewDrawPass(DrawBuffer* buffer,
	Ref<ShaderPipeline> pipeline, Ref<VolcaniCore::Framebuffer> output)
{
	// for(auto& pass : s_Data.Passes)
	// 	if(pass.BufferData == buffer && pass.Pipeline == pipeline)
	// 		return &pass;

	return &s_Data.Passes.Emplace(buffer, pipeline, output);
}

DrawCommand* Renderer::NewDrawCommand(DrawPass* pass) {
	auto* command = &s_Data.Commands.Emplace(pass);

	if(pass && pass->BufferData) {
		command->IndicesIndex = pass->BufferData->IndicesCount;
		command->VerticesIndex = pass->BufferData->VerticesCount;
		command->InstancesIndex = pass->BufferData->InstancesCount;
	}

	return command;
}

void FlushCommand(DrawCommand& command) {
	if(command.Pass)
		SetOptions(command);

	if(command.ViewportWidth && command.ViewportHeight)
		Resize(command.ViewportWidth, command.ViewportHeight);
	else if(command.Pass && command.Pass->Output)
		Resize(command.Pass->Output->GetWidth(), command.Pass->Output->GetHeight());

	if(command.Pass && command.Pass->Output)
		command.Pass->Output->As<OpenGL::Framebuffer>()->Bind();

	if(command.Clear)
		Clear();

	if(command.Pass && command.ComputeX && command.ComputeY && command.ComputeZ)
		command.Pass->Pipeline->As<OpenGL::ShaderProgram>()->Lock();

	if(command.Pass && command.Pass->Pipeline) {
		command.Pass->Pipeline->As<OpenGL::ShaderProgram>()->Bind();
		SetUniforms(command);
	}

	if(command.Pass && command.Pass->Output) {
		uint32_t i = 0;
		for(auto& [target, idx] : command.Outputs)
			command.Pass->Output->Attach(target, idx, i++);
	}

	Ref<VertexArray> array;
	if(command.Pass && command.Pass->BufferData) {
		array = s_Data.Arrays[command.Pass->BufferData].Array;
		array->Bind();
	}

	for(auto& call : command.Calls) {
		FlushCall(command, call);
		s_Info.IndexCount += call.IndexCount;
		s_Info.VertexCount += call.VertexCount;
		s_Info.InstanceCount += call.InstanceCount;
		s_Info.DrawCallCount++;
	}
	if(command.Pass && command.ComputeX && command.ComputeY && command.ComputeZ)
		command.Pass->Pipeline->As<OpenGL::ShaderProgram>()
			->Compute(command.ComputeX, command.ComputeY, command.ComputeZ);

	if(command.Pass && command.Pass->BufferData)
		array->Unbind();

	Resize(Application::GetWindow()->GetWidth(),
			Application::GetWindow()->GetHeight());

	if(command.Pass && command.Pass->Output)
		command.Pass->Output->As<OpenGL::Framebuffer>()->Unbind();
	if(command.Pass && command.Pass->Pipeline)
		command.Pass->Pipeline->As<OpenGL::ShaderProgram>()->Unbind();
}

void FlushCall(DrawCommand& command, DrawCall& call) {
	uint32_t primitive;
	switch(call.Primitive) {
		case PrimitiveType::Point:
			primitive = GL_POINTS;
			break;
		case PrimitiveType::Line:
			primitive = GL_LINES;
			break;
		case PrimitiveType::Triangle:
			primitive = GL_TRIANGLES;
			break;
	}

	if(call.Partition == PartitionType::MultiDraw) {
		// if(call.IndexCount == 0)
		// 	glMultiDrawArraysIndirect();
		// else
		// 	glMultiDrawElementsIndirect();

		// Add indirect objects. When full, make draw call and clear
		return;
	}

	if(call.Partition == PartitionType::Single) {
		if(call.IndexCount == 0)
			glDrawArrays(primitive, command.VerticesIndex + call.VertexStart,
						 call.VertexCount);
		else
			glDrawElementsBaseVertex(
				primitive, call.IndexCount, GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * (command.IndicesIndex + call.IndexStart)),
				command.VerticesIndex + call.VertexStart);
	}
	else if(call.Partition == PartitionType::Instanced) {
		if(call.IndexCount == 0)
			glDrawArraysInstancedBaseInstance(
				primitive, command.VerticesIndex + call.VertexStart,
				call.VertexCount, call.InstanceCount, call.InstanceStart);
		else
			glDrawElementsInstancedBaseVertexBaseInstance(
				primitive, call.IndexCount, GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * (command.IndicesIndex + call.IndexStart)),
				call.InstanceCount, command.VerticesIndex + call.VertexStart,
				call.InstanceStart);
	}
}

void Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, (int)width, (int)height);
}

void SetUniforms(DrawCommand& command) {
	auto& uniforms = command.UniformData;
	auto shader = command.Pass->Pipeline;

	for(auto& [name, data] : uniforms.IntUniforms)
		shader->SetInt(name, data);
	for(auto& [name, data] : uniforms.FloatUniforms)
		shader->SetFloat(name, data);
	for(auto& [name, slot] : uniforms.TextureUniforms) {
		if(!slot.Sampler)
			continue;
		slot.Sampler->As<OpenGL::Texture2D>()->Bind(slot.Index);
		shader->SetInt(name, slot.Index);
	}
	for(auto& [name, data] : uniforms.Vec2Uniforms)
		shader->SetVec2(name, data);
	for(auto& [name, data] : uniforms.Vec3Uniforms)
		shader->SetVec3(name, data);
	for(auto& [name, data] : uniforms.Vec4Uniforms)
		shader->SetVec4(name, data);
	for(auto& [name, data] : uniforms.Mat2Uniforms)
		shader->SetMat2(name, data);
	for(auto& [name, data] : uniforms.Mat3Uniforms)
		shader->SetMat3(name, data);
	for(auto& [name, data] : uniforms.Mat4Uniforms)
		shader->SetMat4(name, data);

	for(auto& [buffer, name, binding] : uniforms.UniformBuffers) {
		if(name != "")
			shader->SetUniformBuffer(name, binding);
		buffer->As<OpenGL::UniformBuffer>()->Bind(binding);
	}

	for(auto& [buffer, name, binding] : uniforms.StorageBuffers) {
		if(name != "")
			shader->SetStorageBuffer(name, binding);
		buffer->As<OpenGL::StorageBuffer>()->Bind(binding);
	}
}

void SetOptions(DrawCommand& command) {
	if(command.DepthTest == DepthTestingMode::On)
		glEnable(GL_DEPTH_TEST);
	if(command.DepthTest == DepthTestingMode::Off)
		glDisable(GL_DEPTH_TEST);

	if(command.Blending == BlendingMode::Off)
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);
	if(command.Blending == BlendingMode::Greatest)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if(command.Blending == BlendingMode::Additive) {
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	if(command.Culling == CullingMode::Off)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
	if(command.Culling == CullingMode::Front)
		glCullFace(GL_FRONT);
	else if(command.Culling == CullingMode::Back)
		glCullFace(GL_BACK);
}

}