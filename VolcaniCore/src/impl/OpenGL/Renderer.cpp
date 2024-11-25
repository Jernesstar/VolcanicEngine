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
	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	List<BackendBuffer> Arrays;
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

	float cubemapVertices[] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float framebufferCoords[] =
	{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	Ref<VertexBuffer> cubemapBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Position", BufferDataType::Vec3 }
		},
		cubemapVertices
	);
	s_Data.CubemapArray = CreatePtr<VertexArray>(cubemapBuffer);

	Ref<VertexBuffer> buffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "TexCoord", BufferDataType::Vec2 }
		},
		framebufferCoords
	);
	s_Data.FramebufferArray = CreatePtr<VertexArray>(buffer);

	s_Data.GeometryBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{
				{ "Coordinate", BufferDataType::Vec3 },
				{ "Normal",		BufferDataType::Vec3 },
				{ "TexCoord_Color", BufferDataType::Vec4 },
			},
			true, // Dynamic
			false // Structure of arrays
		},
		VolcaniCore::Renderer::MaxVertices
	);
	// TODO(Change): Turn into MappedBuffer
	s_Data.TransformBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ { "Transform", BufferDataType::Mat4 } },
			true, // Dynamic
			true // Structure of arrays
		},
		VolcaniCore::Renderer::MaxInstances
	);
	s_Data.Indices =
		CreateRef<IndexBuffer>(VolcaniCore::Renderer::MaxIndices, true);

	s_Data.Array = CreatePtr<VertexArray>();
	s_Data.Array->SetIndexBuffer(s_Data.Indices);
	s_Data.Array->AddVertexBuffer(s_Data.GeometryBuffer);
	s_Data.Array->AddVertexBuffer(s_Data.TransformBuffer);
}

void Renderer::Close() {
	s_Data.Arrays.clear();
}

static void Clear();
static void Resize(uint32_t width, uint32_t height);
static void SetOptions(DrawCommand& command);

void Renderer::StartFrame() {
	Clear();
}

void Renderer::EndFrame() {

}

DrawCommand CreateDrawCommand(BufferLayout vertex, BufferLayout instance) {
	for(auto array : s_Data.Arrays) {
		if(array->GetVertexBuffers()[0] == )
	}
}

void SubmitDrawCommand(DrawCommand& command) {

}

static void SetOptions(DrawCommand& command) {
	if(options.DepthTest == RendererAPI::Options::DepthTestingMode::On)
		glEnable(GL_DEPTH_TEST);
	if(options.DepthTest == RendererAPI::Options::DepthTestingMode::Off)
		glDisable(GL_DEPTH_TEST);

	if(options.Blending == RendererAPI::Options::BlendingMode::Off)
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);
	if(options.Blending == RendererAPI::Options::BlendingMode::Greatest)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if(options.Blending == RendererAPI::Options::BlendingMode::Additive) {
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	if(options.Cull == RendererAPI::Options::CullingMode::Off)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
	if(options.Cull == RendererAPI::Options::CullingMode::Front)
		glCullFace(GL_FRONT);
	else if(options.Cull == RendererAPI::Options::CullingMode::Back)
		glCullFace(GL_BACK);
}

void Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

static void DrawPoint(DrawCall& call) {
	auto& geometry = call.GeometryBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t instanceCount = transforms.GetCount();

	s_Data.GeometryBuffer->SetData(geometry);
	s_Data.TransformBuffer->SetData(transforms);

	glDrawArrays(GL_POINTS, 0, instanceCount);
}

// TODO(Implement):
static void DrawLine(DrawCall& call) {
	auto& geometry = call.GeometryBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t instanceCount = transforms.GetCount();

	s_Data.GeometryBuffer->SetData(geometry);
	s_Data.TransformBuffer->SetData(transforms);

	glDrawArrays(GL_LINES, 0, 2);
}

static void DrawMesh(DrawCall& call) {
	if(call.Partition == DrawPartition::Multi) {
		// TODO(Implement): Pack meshes together and issue a single draw call
		return;
	}

	auto& indices	 = call.IndexBuffer;
	auto& geometry	 = call.GeometryBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t indexCount = indices.GetCount();
	uint32_t instanceCount = transforms.GetCount();

	s_Data.GeometryBuffer->SetData(geometry);

	if(call.Type == DrawType::Indexed)
		s_Data.Indices->SetData(indices);

	if(call.Partition == DrawPartition::Single) {
		if(call.Type == DrawType::Array)
			glDrawArrays(GL_TRIANGLES, 0, indexCount);
		else
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	else if(call.Partition == DrawPartition::Instanced) {
		s_Data.TransformBuffer->SetData(transforms);

		if(call.Type == DrawType::Array)
			glDrawArraysInstanced(GL_TRIANGLES, 0, indexCount, instanceCount);
		else
			glDrawElementsInstanced(GL_TRIANGLES, indexCount,
									GL_UNSIGNED_INT, 0, instanceCount);
	}
}

static void SubmitDrawCall(DrawCall& call) {
	switch(call.Primitive) {
		case DrawPrimitive::Point:
			DrawPoint(call);
			break;
		case DrawPrimitive::Line:
			DrawLine(call);
			break;
		case DrawPrimitive::Mesh:
			DrawMesh(call);
			break;
	}
}

static void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);

	cubemap->As<OpenGL::Cubemap>()->Bind();
	s_Data.CubemapArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

// TODO(Change): Move to VolcaniCore::Renderer2D::DrawFullScreenQuad
static void RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer, AttachmentTarget target)
{
	if(!buffer) {
		VOLCANICORE_LOG_INFO("WHYYY?");
	}
	if(!buffer->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

	auto pass = VolcaniCore::Renderer::GetPass();
	Ref<ShaderPipeline> pipeline = nullptr;
	Ref<VolcaniCore::Framebuffer> output = nullptr;
	if(pass) {
		pipeline = pass->GetPipeline();
		output = pass->GetOutput();
	}

	if(!pipeline)
		ShaderLibrary::Get("Framebuffer")->Bind();
	// else
	// 	pipeline->Bind();

	if(output) {
		output->Bind();
		Resize(output->GetWidth(), output->GetHeight());
	}
	else
		buffer->Bind(target, 0);

	s_Data.FramebufferArray->Bind();
	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	s_Data.FramebufferArray->Unbind();

	if(output) {
		auto window = Application::GetWindow();
		Resize(window->GetWidth(), window->GetHeight());
		output->Unbind();
	}
	// if(pipeline)
	// 	pipeline->Unbind();
}

}