#include "Renderer.h"

#include <unordered_map>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"
#include "Events/EventSystem.h"

#include "Renderer/ShaderLibrary.h"

#include "Shader.h"

#include "Mesh.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "Framebuffer.h"

#include "VertexArray.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

Renderer::Renderer()
	: RendererAPI(RendererBackend::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});
}

static const uint32_t MaxInstances = 1000;

struct RendererData {
	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	Ref<VertexBuffer> MeshBuffer;
	Ref<VertexBuffer> LineBuffer;
	Ref<VertexBuffer> PointBuffer;

	Ptr<VertexArray> MeshArray;
	Ptr<VertexArray> LineArray;
	Ptr<VertexArray> PointArray;
};

static RendererData s_Data;

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

	float framebufferVertices[] =
	{
		// Coords      // TexCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	Ref<VertexBuffer> cubemapBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Position", BufferDataType::Vec3 }
		}, cubemapVertices
	);
	s_Data.CubemapArray = CreatePtr<VertexArray>(cubemapBuffer);

	Ref<VertexBuffer> buffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Coordinate", BufferDataType::Vec2 },
			{ "TexCoord",	BufferDataType::Vec2 },
		}, framebufferVertices);
	s_Data.FramebufferArray = CreatePtr<VertexArray>(buffer);

	// s_Data.MeshBuffer = CreateRef<VertexBuffer>(
	// 	BufferLayout{
	// 		{ "Coordinate", BufferDataType::Vec3 },
	// 		{ "Normal",		BufferDataType::Vec3 },
	// 		{ "TexCoord_Color", BufferDataType::Vec4 },
	// 	});
	// s_Data.MeshArray = CreatePtr<VertexArray>(buffer);

	// s_Data.LineBuffer = CreateRef<VertexBuffer>(
	// 	BufferLayout{
	// 		{ "Coordinate", BufferDataType::Vec3 },
	// 		{ "Color",		BufferDataType::Vec3 },
	// 	});
	// s_Data.LineArray = CreatePtr<VertexArray>(buffer);

	// s_Data.PointBuffer = CreateRef<VertexBuffer>(
	// 	BufferLayout{
	// 		{ "Coordinate", BufferDataType::Vec3 },
	// 		{ "Color",		BufferDataType::Vec3 },
	// 	});
	// s_Data.PointArray = CreatePtr<VertexArray>(buffer);

	ShaderLibrary::Get("Framebuffer")->Bind();
	ShaderLibrary::Get("Framebuffer")->SetInt("u_ScreenTexture", 0);
}

void Renderer::Close() { }

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void Renderer::DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);

	cubemap->As<OpenGL::Cubemap>()->Bind();
	s_Data.CubemapArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Renderer::DrawMesh(Ref<VolcaniCore::Mesh> mesh, const glm::mat4& transform)
{
	auto nativeMesh = mesh->As<OpenGL::Mesh>();

	DrawIndexed(nativeMesh->m_VertexArray);
}

void Renderer::DrawLine(Ref<VolcaniCore::Line> line, const glm::mat4& transform)
{
	// s_Data.LineVAO->Bind();
	// glDrawArrays(GL_LINES, 0, 2);
}

void Renderer::DrawPoint(Ref<VolcaniCore::Point> point, const glm::mat4& tr) {

}

void Renderer::RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer,
								 AttachmentTarget target)
{
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	if(!buffer->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

	ShaderLibrary::Get("Framebuffer")->Bind();
	buffer->As<OpenGL::Framebuffer>()->Get(target).Bind();
	s_Data.FramebufferArray->Bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawIndexed(Ref<VertexArray> array, uint32_t indices) {
	if(!array->HasIndexBuffer()) {
		VOLCANICORE_LOG_WARNING("Attempt to execute indexed draw call \
								without index buffer bound has failed");
		return;
	}
	uint32_t count = array->GetIndexBuffer()->Count;

	array->Bind();
	glDrawElements(GL_TRIANGLES, indices != 0 ? indices : count,
				   GL_UNSIGNED_INT, 0);
}

void Renderer::DrawInstanced(Ref<VertexArray> array, uint32_t instanceCount) {
	if(!array->HasIndexBuffer()) {
		VOLCANICORE_LOG_WARNING("Attempt to execute instanced draw call \
								 without index buffer bound has failed");
		return;
	}
	uint32_t count = array->GetIndexBuffer()->Count;

	array->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, count,
							GL_UNSIGNED_INT, 0, instanceCount);
}

}