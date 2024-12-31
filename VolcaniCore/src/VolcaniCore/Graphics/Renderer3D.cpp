#include "Renderer3D.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

static DrawBuffer* s_CubemapBuffer;
static DrawBuffer* s_MeshBuffer;
// static DrawBuffer* s_PointBuffer;
static Map<Ref<Mesh>, DrawCommand*> s_Meshes;

void Renderer3D::Init() {
	BufferLayout vertexLayout =
		{
			{
				{ "Position", BufferDataType::Vec3 },
				{ "Normal",	  BufferDataType::Vec3 },
				{ "TexCoord", BufferDataType::Vec2 },
			},
			true, // Dynamic
			false // Structure of arrays
		};

	BufferLayout instanceLayout =
		{
			{
				{ "Transform", BufferDataType::Mat4 }
			},
			true, // Dynamic
			true  // Structure of arrays
		};

	DrawBufferSpecification specs
	{
		vertexLayout,
		instanceLayout,
		Renderer::MaxVertices,
		Renderer::MaxIndices,
		Renderer::MaxInstances
	};

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

	BufferLayout cubemapLayout =
		{
			{
				{ "Position", BufferDataType::Vec3 },
			},
			true, // Dynamic
			false // Structure of arrays
		};
	DrawBufferSpecification specsCubemap
		{
			.VertexLayout = cubemapLayout,
			.MaxVertexCount = 36
		};

	s_MeshBuffer = RendererAPI::Get()->NewDrawBuffer(specs);
	s_CubemapBuffer =
		RendererAPI::Get()->NewDrawBuffer(specsCubemap, cubemapVertices);
}

void Renderer3D::Close() {
	RendererAPI::Get()->ReleaseBuffer(s_CubemapBuffer);
	RendererAPI::Get()->ReleaseBuffer(s_MeshBuffer);
}

void Renderer3D::StartFrame() {
	s_MeshBuffer->Clear();
}

void Renderer3D::EndFrame() {
	s_Meshes.clear();
}

void Renderer3D::Begin(Ref<Camera> camera) {
	auto* command = Renderer::GetCommand();
	command->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	command->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void Renderer3D::End() {
	Renderer::EndCommand();
}

void Renderer3D::DrawSkybox(Ref<Cubemap> cubemap) {
	auto* command = Renderer::NewCommand();
	auto& call = command->NewDrawCall();

	// call.DepthMask = false;
	call.Partition = PartitionType::Single;
	call.VertexCount = 36;

	// command->UniformData
	// .SetInput("u_Skybox", TextureSlot{ cubemap, 0 });
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr) {
	if(!mesh)
		return;

	if(!s_Meshes.count(mesh)
	|| s_Meshes[mesh]->Calls[0].InstanceCount >= 10'000)
	{
		auto pass = Renderer::GetPass();
		pass->SetData(s_MeshBuffer);

		auto* command = Renderer::NewCommand();
		command->ViewportWidth = 1920;
		command->ViewportHeight = 1080;
		command->InstancesIndex = s_Meshes.size() * 10'000;

		auto& call = command->NewDrawCall();
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.IndexCount  = mesh->GetIndices().size();
		call.VertexCount = mesh->GetVertices().size();

		command->AddIndices(Buffer(mesh->GetIndices()));
		command->AddVertices(Buffer(mesh->GetVertices()));

		Material& mat = mesh->GetMaterial();
		command->UniformData
		.SetInput("u_Diffuse", TextureSlot{ mat.Diffuse, 0 });
		command->UniformData
		.SetInput("u_Specular", TextureSlot{ mat.Specular, 1 });

		s_Meshes[mesh] = command;
	}

	auto* command = s_Meshes[mesh];
	command->AddInstance(glm::value_ptr(tr));
	command->Calls[0].InstanceCount++;
}

void Renderer3D::DrawModel(Ref<Model> model, const glm::mat4& tr) {
	for(auto& mesh : *model)
		DrawMesh(mesh, tr);
}

void Renderer3D::DrawQuad(Ref<Quad> quad, const glm::mat4& tr) {
	Ref<Mesh> mesh;
	if(quad->IsTextured)
		mesh = Mesh::Create(MeshPrimitive::Quad,
			Material{ .Diffuse = quad->GetTexture() });
	else
		mesh = Mesh::Create(MeshPrimitive::Quad, quad->GetColor());

	DrawMesh(mesh, tr);
}

void Renderer3D::DrawQuad(Ref<Texture> texture, const glm::mat4& tr) {
	DrawQuad(Quad::Create(texture), tr);
}

void Renderer3D::DrawQuad(const glm::vec4& color, const glm::mat4& tr) {
	DrawQuad(Quad::Create(1, 1, color), tr);
}

void Renderer3D::DrawPoint(const Point& point, const glm::mat4& tr) {
	auto* command = Renderer::GetCommand();

}

void Renderer3D::DrawLine(const Line& line, const glm::mat4& tr) {
	// auto* command = Renderer::GetCommand();

}

void Renderer3D::DrawText(Ref<Text> text, const glm::mat4& tr) {
	// TODO(Implement)
}

}