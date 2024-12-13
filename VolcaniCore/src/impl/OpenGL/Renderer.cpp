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
#include "Mesh.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "Framebuffer.h"

#include "VertexArray.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

struct BackendBuffer {
	Ref<VertexArray> Array;
	Buffer<uint32_t> Indices;
	Buffer<void> Vertices;
	Buffer<void> Instances;
};

struct RendererData {
	Map<DrawBuffer*, BackendBuffer> Arrays;
	List<DrawCommand> Commands;
};

static RendererData s_Data;

Renderer::Renderer()
	: RendererAPI(RendererAPI::Backend::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");
}

void Renderer::Init() {
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Smooth cubemap edges
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Renderer::Close() {
	s_Data.Arrays.clear();
}

static void Clear();
static void Resize(uint32_t width, uint32_t height);

static void FlushCommand(DrawCommand& command);
static void FlushCall(DrawCall& call);

static void SetUniforms(DrawCommand& command);
static void SetOptions(DrawCall& call);

void Renderer::StartFrame() {

}

void Renderer::EndFrame() {
	if(!s_Data.Commands.size()) // EndFrame already called
		return;

	for(auto& [buffer, backend] : s_Data.Arrays) {
		if(backend.Indices.GetCount() && buffer->Specs.MaxIndexCount)
			backend.Array->GetIndexBuffer()->SetData(backend.Indices);
		if(backend.Vertices.GetCount() && buffer->Specs.MaxVertexCount)
			backend.Array->GetVertexBuffer(0)->SetData(backend.Vertices);
		if(backend.Instances.GetCount() && buffer->Specs.MaxInstanceCount)
			backend.Array->GetVertexBuffer(1)->SetData(backend.Instances);
	}

	for(auto& command : s_Data.Commands)
		FlushCommand(command);

	s_Data.Commands.clear();
}

DrawBuffer* Renderer::NewDrawBuffer(DrawBufferSpecification& specs, void* data) {
	auto* buffer = new DrawBuffer{ specs };
	auto array = CreateRef<VertexArray>();
	s_Data.Arrays[buffer] = BackendBuffer{ array };

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
}

DrawCommand* Renderer::NewDrawCommand(DrawBuffer* buffer) {
	return &s_Data.Commands.emplace_back(buffer);
}

void FlushCommand(DrawCommand& command) {
	command.Pipeline->As<OpenGL::ShaderProgram>()->Bind();

	if(command.Image)
		command.Image->As<OpenGL::Framebuffer>()->Bind();

	if(command.ViewportWidth != 0 && command.ViewportHeight != 0)
		Resize(command.ViewportWidth, command.ViewportHeight);
	if(command.Clear)
		Clear();

	SetUniforms(command);

	if(command.BufferData) {
		auto array = s_Data.Arrays[command.BufferData].Array;

		array->Bind();

		for(auto& call : command.Calls)
			FlushCall(call);

		array->Unbind();
	}

	if(command.Image)
		command.Image->As<OpenGL::Framebuffer>()->Unbind();

	command.Pipeline->As<OpenGL::ShaderProgram>()->Unbind();
}

void FlushCall(DrawCall& call) {
	SetOptions(call);

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
	}
	else if(call.Partition == PartitionType::Single) {
		if(call.IndexCount == 0)
			glDrawArrays(primitive, call.VertexStart, call.VertexCount);
		else
			glDrawElementsBaseVertex(primitive, call.IndexCount,
									 GL_UNSIGNED_INT, 0, call.IndexStart);
	}
	else if(call.Partition == PartitionType::Instanced) {
		if(call.IndexCount == 0)
			glDrawArraysInstancedBaseInstance(
				primitive, call.VertexStart, call.VertexCount,
				call.InstanceCount, call.InstanceStart);
		else
			glDrawElementsInstancedBaseVertexBaseInstance(
				primitive, call.IndexCount, GL_UNSIGNED_INT, 0,
				call.InstanceCount, call.IndexStart, call.InstanceStart);
	}
}

void Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void SetUniforms(DrawCommand& command) {
	auto& uniforms = command.UniformData;
	auto shader = command.Pipeline;

	for(auto& [name, data] : uniforms.IntUniforms)
		shader->SetInt(name, data);
	for(auto& [name, data] : uniforms.FloatUniforms)
		shader->SetFloat(name, data);
	for(auto& [name, slot] : uniforms.TextureUniforms)
		shader->SetTexture(name, slot.Sampler, slot.Index);
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
}

void SetOptions(DrawCall& call) {
	if(call.DepthTest == DepthTestingMode::On)
		glEnable(GL_DEPTH_TEST);
	if(call.DepthTest == DepthTestingMode::Off)
		glDisable(GL_DEPTH_TEST);

	if(call.Blending == BlendingMode::Off)
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);
	if(call.Blending == BlendingMode::Greatest)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if(call.Blending == BlendingMode::Additive) {
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	if(call.Culling == CullingMode::Off)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
	if(call.Culling == CullingMode::Front)
		glCullFace(GL_FRONT);
	else if(call.Culling == CullingMode::Back)
		glCullFace(GL_BACK);
}

}