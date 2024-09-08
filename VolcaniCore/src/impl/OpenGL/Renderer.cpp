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

struct RendererData {
	static const uint32_t MaxInstances = 1000;

	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	Ref<VertexArray> Array;
	Ref<VertexBuffer> GeometryBuffer;
	Ref<VertexBuffer> IndexBuffer;
	Ref<VertexBuffer> TransformBuffer;
};

static RendererData s_Data;

Renderer::Renderer()
	: RendererAPI(RendererAPI::Backend::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

	Events::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});
}

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);				// Depth testing
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // ???

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glCullFace(GL_BACK);

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
		// TexCoords
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
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
			{ "Coordinate", BufferDataType::Vec3 },
			{ "Normal",		BufferDataType::Vec3 },
			{ "TexCoord_Color", BufferDataType::Vec4 },
		},
		RendererData::MaxInstances
	);
	// TODO(Change): Turn into MappedBuffer
	s_Data.TransformBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Transform", BufferDataType::Mat4 }
		},
		RendererData::MaxInstances
	);
	s_Data.IndexBuffer = CreateRef<IndexBuffer>();

	s_Data.Array = CreatePtr<VertexArray>();
	s_Data.Array->AddVertexBuffer(s_Data.GeometryBuffer);
	s_Data.Array->AddVertexBuffer(s_Data.TransformBuffer);
	s_Data.Array->SetIndexBuffer(s_Data.IndexBuffer);

	ShaderLibrary::Get("Framebuffer")->Bind();
	ShaderLibrary::Get("Framebuffer")->SetInt("u_ScreenTexture", 0);
}

void Renderer::Close() { }

void Renderer::StartFrame() {

}
void Renderer::EndFrame() {

}

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

// TODO(Implement):
static void DrawPoint(DrawCall& call) {
	auto& geometry = call.GeometryBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t instanceCount = transforms->GetCount();

	s_Data.Array->Bind();
	s_Data.GeometryBuffer->SetData(geometry);
	s_Data.TransformBuffer->SetData(transforms);

	glDrawArrays(GL_POINTS, 0, instanceCount);
}

// TODO(Implement):
static void DrawLine(DrawCall& call) {
	auto& geometry = call.GeometryBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t instanceCount = transforms->GetCount();

	s_Data.Array->Bind();
	s_Data.GeometryBuffer->SetData(geometry);
	s_Data.TransformBuffer->SetData(transforms);

	glDrawArrays(GL_LINES, 0, 2);
}

static void DrawMesh(DrawCall& call) {
	if(call.Options.Partition == DrawPartition::Multi) {
		// TODO(Implement): Pack meshes together and issue a single draw call
		return;
	}

	auto& geometry	 = call.GeometryBuffer;
	auto& indices	 = call.IndexBuffer;
	auto& transforms = call.TransformBuffer;
	uint32_t count = indices->GetCount();
	uint32_t instanceCount = transforms->GetCount();

	s_Data.Array->Bind();
	s_Data.GeometryBuffer->SetData(geometry);
	s_Data.IndexBuffer->SetData(indices);
	s_Data.TransformBuffer->SetData(transforms);

	switch(call.Partition) {
		case DrawPartition::Single:
		{
			if(call.Type == Array)
				glDrawArrays(GL_TRIANGLES, 0, count);
			else
				glDrawElements(GL_TRIANGLES, 0, count);

			break;
		}
		case DrawPartition::Instanced:
		{
			s_Data.TransformBuffer->SetData(trs);

			if(call.Type == Array)
				glDrawArraysInstanced(GL_TRIANGLES, count,
									  GL_UNSIGNED_INT, 0, instanceCount);
			else
				glDrawElementsInstanced(GL_TRIANGLES, count,
										GL_UNSIGNED_INT, 0, instanceCount);

			break;
		}
	}
}

void Renderer::SubmitDrawCall(DrawCall& call) {
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

void Renderer::RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);

	cubemap->As<OpenGL::Cubemap>()->Bind();
	s_Data.CubemapArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Renderer::RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer,
								 AttachmentTarget target)
{
	if(!buffer->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

	if(!VolcaniCore::Renderer::GetPass())
		ShaderLibrary::Get("Framebuffer")->Bind();

	buffer->As<OpenGL::Framebuffer>()->Get(target).Bind();
	s_Data.FramebufferArray->Bind();

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

}