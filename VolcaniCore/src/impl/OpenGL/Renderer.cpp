#include "Renderer.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <Core/Application.h>
#include <Core/Assert.h>
#include <Event/Events.h>

#include <Renderer/Renderer.h>
#include <Renderer/ShaderLibrary.h>

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
	Buffer<void> Vertices;
	Buffer<uint32_t> Indices;
	Buffer<void> InstanceData;
};

struct RendererData {
	Map<DrawBuffer*, BackendBuffer> Arrays;
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
static void SetOptions(DrawCall& call);
static void SetUniforms(DrawCall& call);

static void DrawPoint(DrawCall& call);
static void DrawLine(DrawCall& call);
static void DrawMesh(DrawCall& call);

void Renderer::StartFrame() {

}

void Renderer::EndFrame() {

}

DrawBuffer* Renderer::NewDrawBuffer(DrawBufferSpecification& specs,
									void* data)
{
	auto* buffer = new DrawBuffer{ specs };

	auto array = CreateRef<VertexArray>();
	array->SetIndexBuffer(CreateRef<IndexBuffer>(specs.MaxIndexCount));

	if(data) {
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(
				specs.VertexLayout, specs.MaxVertexCount, data)
		);
		s_Data.Arrays[buffer] = BackendBuffer{ array }; // Static Buffer
	}
	else {
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(specs.VertexLayout, specs.MaxVertexCount));

		// TODO(Fix): Replace with emplace
		s_Data.Arrays[buffer] =
			BackendBuffer
			{
				array,
				Buffer<void>(specs.MaxVertexCount),
				Buffer<uint32_t>(specs.MaxIndexCount),
				Buffer<void>(specs.MaxInstanceCount)
			};
	}

	if(specs.InstanceLayout != BufferLayout{ })
		array->AddVertexBuffer(
			CreateRef<VertexBuffer>(specs.InstanceLayout, specs.MaxInstanceCount)
		);

	return buffer;
}

DrawBuffer* Renderer::GetDrawBuffer(DrawBufferSpecification& specs) {
	auto vertexLayout = specs.VertexLayout;
	auto instanceLayout = specs.InstanceLayout;
	for(auto& [buffer, backendBuffer] : s_Data.Arrays)
		if(backendBuffer.Array->GetVertexBuffer(0)->Layout == vertexLayout
		&& backendBuffer.Array->GetVertexBuffer(1)->Layout == instanceLayout)
			return buffer;

	return NewDrawBuffer(specs);
}

void Renderer::Submit(DrawCall& call) {
	call.Pipeline->As<OpenGL::ShaderProgram>()->Bind();

	auto array = s_Data.Arrays[call.Buffer].Array;
	array->GetVertexBuffer(0)->SetData(call.Buffer->Vertices);

	if(call.IndexCount)
		array->GetIndexBuffer()->SetData(call.Buffer->Indices);
	if(call.InstanceCount)
		array->GetVertexBuffer(1)->SetData(call.Buffer->Instances);

	SetOptions(call);
	SetUniforms(call);

	switch(call.Primitive) {
		case PrimitiveType::Point:
			DrawPoint(call);
			break;
		case PrimitiveType::Line:
			DrawLine(call);
			break;
		case PrimitiveType::Triangle:
			DrawMesh(call);
			break;
	}
}

static void DrawPoint(DrawCall& call) {
	uint32_t instanceCount = call.InstanceCount;

	glDrawArrays(GL_POINTS, 0, instanceCount);
}

// TODO(Implement):
static void DrawLine(DrawCall& call) {
	uint32_t instanceCount = call.InstanceCount;

	glDrawArraysInstanced(GL_LINES, 0, 2, instanceCount);
}

static void DrawMesh(DrawCall& call) {
	if(call.Partition == PartitionType::MultiDraw) {
		// TODO(Implement): Pack meshes together and issue a single draw call
		// return;
	}
	else if(call.Partition == PartitionType::Single) {
		if(call.IndexCount == 0)
			glDrawArrays(GL_TRIANGLES, call.VertexStart, call.VertexCount);
		else
			glDrawElements(GL_TRIANGLES, call.IndexCount,
						   GL_UNSIGNED_INT, nullptr);
	}
	else if(call.Partition == PartitionType::Instanced) {
		if(call.IndexCount == 0)
			glDrawArraysInstanced(GL_TRIANGLES, call.VertexStart,
								  call.VertexCount, call.InstanceCount);
		else
			glDrawElementsInstanced(GL_TRIANGLES, call.IndexCount,
									GL_UNSIGNED_INT, nullptr, call.InstanceCount);
	}
}

static void SetOptions(DrawCall& call) {
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

static void SetUniforms(DrawCall& call) {
	// TODO(Implement):
}

static void Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

}