#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Assert.h"
#include "Events/EventSystem.h"

#include "Renderer/OrthographicCamera.h"

#include "Shader.h"

#include "Mesh.h"
#include "Model.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "Framebuffer.h"

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

struct RendererData {
	// static const uint32_t MaxQuads = 150;
	// static const uint32_t MaxVertices = MaxQuads * 4;
	// static const uint32_t MaxIndices = MaxQuads * 6;
	// static const uint32_t MaxTextureSlots = 32;

	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	Ref<ShaderPipeline> FramebufferShader;

	Ref<VertexBuffer> MeshBuffer;
	Ptr<VertexArray> MeshArray;
	Ref<ShaderPipeline> MeshShader;

	std::vector<Ref<Quad>> Quads;
	std::unordered_map<Ref<Mesh>, uint32_t> Meshes;
};

static RendererData s_Data;

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);				// Depth testing
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // ???

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

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
		// Coords    // TexCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};


	// MeshBuffer = CreateRef<VertexBuffer>(
	// 	BufferLayout{
	// 		{ "Position", BufferDataType::Vec3 }
	// 		{ "", BufferDataType::Vec3 }
	// 		{ "Position", BufferDataType::Vec3 }
	// 	}, cubemapVertices
	// );
	// s_Data.CubemapArray = CreatePtr<VertexArray>(cubemapBuffer);

	Ref<VertexBuffer> cubemapBuffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Position", BufferDataType::Vec3 }
		}, cubemapVertices
	);
	s_Data.CubemapArray = CreatePtr<VertexArray>(cubemapBuffer);

	Ref<VertexBuffer> buffer = CreateRef<VertexBuffer>(
		BufferLayout{
			{ "Coordinate", OpenGL::BufferDataType::Vec2 },
			{ "TexCoord",	OpenGL::BufferDataType::Vec2 },
		}, framebufferVertices
	);
	s_Data.FramebufferArray = CreatePtr<VertexArray>(buffer);

	s_Data.FramebufferShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert",
			ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.frag",
			ShaderType::Fragment }
	});
	s_Data.MeshShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});

	s_Data.FramebufferShader->Bind();
	s_Data.FramebufferShader->SetInt("u_ScreenTexture", 0);
}

void Renderer::Close() { }

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	s_Data.ViewportWidth = width;
	s_Data.ViewportHeight = height;
	glViewport(0, 0, width, height);
}

void Renderer::DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);

	s_Data.CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Renderer::DrawMesh(Ref<VolcaniCore::Mesh> mesh, Transform t) {
	auto nativeMesh = mesh->As<OpenGL::Mesh>();
	Material& material = mesh->GetMaterial();

	s_Data.MeshShader->Bind();
	s_Data.MeshShader->SetTexture("u_Diffuse", material.Diffuse, 0);
	// s_Data.MeshShader->SetTexture("u_Specular", material.Specular, 0);
	// s_Data.MeshShader->SetTexture("u_Roughness", material.Roughness, 0);
	s_Data.MeshShader->SetMat4("u_Model", t.GetTransform());

	s_Data.MeshArray->Bind();

	// TODO: Move to RenderCommand
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::Render() {

}

void Renderer::RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer,
								 AttachmentTarget target)
{
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	// ShaderLibrary::Get("Framebuffer")->Bind();
	s_Data.FramebufferShader->Bind();
	if(!buffer->As<OpenGL::Framebuffer>()->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

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
	Ref<IndexBuffer> indexBuffer = array->GetIndexBuffer();

	array->Bind();
	glDrawElements(GL_TRIANGLES, indices != 0 ? indices : indexBuffer->Count,
				   GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawInstanced(Ref<VertexArray> array, uint32_t instanceCount) {
	if(!array->HasIndexBuffer()) {
		VOLCANICORE_LOG_WARNING("Attempt to execute instanced draw call \
								without index buffer bound has failed");
		return;
	}
	Ref<IndexBuffer> indexBuffer = array->GetIndexBuffer();

	array->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, indexBuffer->Count,
							GL_UNSIGNED_INT, 0, instanceCount);
}

}