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

static void DrawIndexed(Ref<VertexArray> vertexArray, uint32_t indices = 0);
static void DrawInstanced(Ref<VertexArray> vertexArray, uint32_t instanceCount);

static const uint32_t MaxInstances = 1000;

struct RendererData {
	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	Ref<VertexArray> MeshArray;
	Ref<VertexArray> LineArray;
	Ref<VertexArray> PointArray;

	Ref<VertexBuffer> Transforms;
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

	auto pointBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Coordinate", BufferDataType::Vec3 },
			{ "Color",		BufferDataType::Vec3 },
		},
		MaxInstances
	);
	s_Data.PointArray = CreatePtr<VertexArray>(pointBuffer);

	auto lineBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Coordinate", BufferDataType::Vec3 },
			{ "Color",		BufferDataType::Vec3 },
		},
		MaxInstances
	);
	s_Data.LineArray = CreatePtr<VertexArray>(lineBuffer);

	// For static geometry
	// s_Data.MeshBuffer = CreateRef<VertexBuffer>(
	// 	BufferLayout{
	// 		{ "Coordinate", BufferDataType::Vec3 },
	// 		{ "Normal",		BufferDataType::Vec3 },
	// 		{ "TexCoord_Color", BufferDataType::Vec4 },
	// 	});
	// s_Data.MeshArray = CreatePtr<VertexArray>(buffer);

	// TODO(Implement): Turn into MappedBuffer
	s_Data.Transforms = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Transform", BufferDataType::Mat4 }
		},
		MaxInstances
	);

	ShaderLibrary::Get("Framebuffer")->Bind();
	ShaderLibrary::Get("Framebuffer")->SetInt("u_ScreenTexture", 0);
}

void Renderer::Close() { }

void Renderer::StartFrame() {

}

void Renderer::RenderPoints(DrawCommand& command) {

}

void Renderer::RenderLines(DrawCommand& command) {

}

void Renderer::RenderMeshes(DrawCommand& command) {

}

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
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

void DrawIndexed(Ref<VertexArray> array, uint32_t indices) {
	if(!array->HasIndexBuffer()) {
		VOLCANICORE_LOG_WARNING("Attempt to execute indexed draw call \
								 without index buffer present has failed");
		return;
	}
	uint32_t count = array->GetIndexBuffer()->Count;

	array->Bind();
	glDrawElements(GL_TRIANGLES, indices != 0 ? indices : count,
					GL_UNSIGNED_INT, 0);
}

void DrawInstanced(Ref<VertexArray> array, uint32_t instanceCount) {
	if(!array->HasIndexBuffer()) {
		VOLCANICORE_LOG_WARNING("Attempt to execute instanced draw call \
								 without index buffer present has failed");
		return;
	}
	uint32_t count = array->GetIndexBuffer()->Count;

	array->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, count,
							GL_UNSIGNED_INT, 0, instanceCount);
}

}