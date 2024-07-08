#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Assert.h"
#include "Events/EventSystem.h"

#include "Renderer/OrthographicCamera.h"

#include "Shader.h"

#include "Object/Quad.h"

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
	static const uint32_t MaxQuads = 150;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> FramebufferArray;

	Ref<ShaderPipeline> FramebufferShader;

	Ref<VertexBuffer> MeshBuffer;
	Ref<VertexBuffer> TransformBuffer;
	Ptr<VertexArray> MeshArray;

	Ref<Mesh> CurrentMesh;
	uint32_t Index = 0;
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

	BufferLayout layout1({
		{ "Position", BufferDataType::Vec3 },
		{ "Normal",   BufferDataType::Vec3 },
		{ "TexCoord", BufferDataType::Vec2 }
	});

	BufferLayout layout2 = {
		{ "a_Transform", BufferDataType::Mat4 }
	};

	MeshBuffer 	= CreateRef<VertexBuffer>(layout1, RendererData::MaxVertices);
	TranformBuffer	= CreateRef<VertexBuffer>(layout2, RendererData::MaxQuads);
	MeshIndexBuffer = CreateRef<IndexBuffer>(MaxInstanceCount * 3);

	MeshArray = CreateRef<VertexArray>();
	MeshArray->SetIndexBuffer(indexBuffer);
	MeshArray->AddVertexBuffer(cubeBuffer);
	MeshArray->AddVertexBuffer(matrixBuffer);

	s_Data.FramebufferShader->Bind();
	s_Data.FramebufferShader->SetInt("u_ScreenTexture", 0);
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

	s_Data.CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Renderer::DrawMesh(Ref<VolcaniCore::Mesh> mesh, Transform t) {
	if(mesh != s_Data.CurrentMesh) {
		Render();
		MeshBuffer->SetData(&mesh.Vertices[0], &mesh.Vertices.size());
		MeshIndexBuffer->SetData(&mesh.Indices[0], &mesh.Indices.size());
	}

	glm::mat4 transform = t.GetTransform();
	TransformBuffer->SetData((void*)&transform, 1, Index++);
}

void Renderer::Render() {
	DrawInstanced(s_Data.MeshArray, Index);
	Index = 0;
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

void Renderer::DrawIndexed(Ptr<VertexArray> array, uint32_t indices) {
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

void Renderer::DrawInstanced(Ptr<VertexArray> array, uint32_t instanceCount) {
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