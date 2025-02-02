#include "Renderer3D.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

static DrawBuffer* s_CubemapBuffer;
static DrawBuffer* s_MeshBuffer;
static Map<Ref<Mesh>, DrawCommand*> s_Meshes;
static uint64_t s_InstanceEnd = 0;

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

DrawBuffer* Renderer3D::GetMeshBuffer() {
	return s_MeshBuffer;
}

DrawBuffer* Renderer3D::GetCubemapBuffer() {
	return s_CubemapBuffer;
}

void Renderer3D::StartFrame() {
	s_MeshBuffer->Clear();
}

void Renderer3D::EndFrame() {
	s_InstanceEnd = 0;
}

void Renderer3D::Begin(Ref<Camera> camera) {
	auto* command = Renderer::GetCommand();
	command->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	command->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void Renderer3D::End() {
	s_Meshes.clear();
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

void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr,
						  DrawCommand* cmd)
{
	if(!mesh)
		return;

	if(!s_Meshes.count(mesh)) {
		if(cmd)
			s_Meshes[mesh] = cmd;
		else {
			s_Meshes[mesh] = Renderer::NewCommand();

			Material& mat = mesh->GetMaterial();
			s_Meshes[mesh]->UniformData
			.SetInput("u_Diffuse", TextureSlot{ mat.Diffuse, 0 });
			s_Meshes[mesh]->UniformData
			.SetInput("u_Specular", TextureSlot{ mat.Specular, 1 });
		}

		s_Meshes[mesh]->AddIndices(mesh->GetIndices().Get());
		s_Meshes[mesh]->AddVertices(mesh->GetVertices().Get());
	}

	DrawCommand* command = s_Meshes[mesh];

	// TODO(Fix): DrawCall offsets should be relative to DrawCommand's
	if(!command->Calls || command->Calls[-1].InstanceCount >= 10'000) {
		auto& call = command->NewDrawCall();
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.IndexCount  = mesh->GetIndices().Count();
		call.VertexCount = mesh->GetVertices().Count();
		call.IndexStart -= mesh->GetIndices().Count();
		call.VertexStart -= mesh->GetVertices().Count();
		call.InstanceStart = s_InstanceEnd;
		s_InstanceEnd += 10'000;
	}

	auto* buffer = Renderer::GetPass()->Get()->BufferData;
	auto& call = command->Calls[-1];
	RendererAPI::Get()
	->SetBufferData(buffer, DrawBufferIndex::Instances, glm::value_ptr(tr),
					1, call.InstanceStart + call.InstanceCount++);
}

void Renderer3D::DrawModel(Ref<Model> model, const glm::mat4& tr,
						   DrawCommand* command)
{
	for(auto& mesh : *model)
		DrawMesh(mesh, tr);
}

void Renderer3D::DrawQuad(Ref<Quad> quad, const glm::mat4& tr,
						  DrawCommand* command)
{
	Ref<Mesh> mesh;
	if(quad->IsTextured)
		mesh = Mesh::Create(MeshPrimitive::Quad,
			Material{ .Diffuse = quad->GetTexture() });
	else
		mesh = Mesh::Create(MeshPrimitive::Quad, quad->GetColor());

	DrawMesh(mesh, tr);
}

void Renderer3D::DrawQuad(Ref<Texture> texture, const glm::mat4& tr,
						  DrawCommand* command)
{
	DrawQuad(Quad::Create(texture), tr);
}

void Renderer3D::DrawQuad(const glm::vec4& color, const glm::mat4& tr,
						  DrawCommand* command)
{
	DrawQuad(Quad::Create(1, 1, color), tr);
}

void Renderer3D::DrawPoint(const Point& point, const glm::mat4& tr,
						   DrawCommand* command)
{
	// TODO(Implement):
}

void Renderer3D::DrawLine(const Line& line, const glm::mat4& tr,
						  DrawCommand* comand)
{
	// TODO(Implement):
}

void Renderer3D::DrawText(Ref<Text> text, const glm::mat4& tr,
						  DrawCommand* command)
{
	// TODO(Implement):
}

}