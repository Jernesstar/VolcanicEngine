#include "Renderer3D.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

static DrawBuffer* s_CubemapBuffer;
static DrawBuffer* s_MeshBuffer;
static Map<SubMesh*, DrawCommand*> s_Meshes;
static uint64_t s_InstancesIndex = 0;

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
	RendererAPI::Get()->ReleaseBuffer(s_MeshBuffer);
	RendererAPI::Get()->ReleaseBuffer(s_CubemapBuffer);
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
	s_InstancesIndex = 0;
}

void Renderer3D::Begin(Ref<Camera> camera) {
	if(!camera)
		return;

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

static void DrawSubMesh(Ref<Mesh> root, SubMesh& mesh, const glm::mat4& tr,
						DrawCommand* cmd)
{
	DrawCommand* command;
	if(s_Meshes.count(&mesh))
		command = s_Meshes[&mesh];
	else {
		if(Renderer::GetPass())
			command = Renderer::NewCommand();
		else {
			command = RendererAPI::Get()->NewDrawCommand(cmd->Pass);
			command->UniformData = cmd->UniformData;
		}

		s_Meshes[&mesh] = command;
	}

	if(!command->VerticesCount) {
		if(cmd) {
			command->IndicesIndex = s_MeshBuffer->IndicesCount;
			command->VerticesIndex = s_MeshBuffer->VerticesCount;
		}

		if(!command->UniformData && root->Materials) {
			Material& mat = root->Materials[mesh.MaterialIndex];
			command->UniformData
			.SetInput("u_Material.IsTextured", (bool)mat.Diffuse);
			command->UniformData
			.SetInput("u_Material.Diffuse", TextureSlot{ mat.Diffuse, 0 });
			command->UniformData
			.SetInput("u_Material.Specular", TextureSlot{ mat.Specular, 1 });
			command->UniformData
			.SetInput("u_Material.Emissive", TextureSlot{ mat.Emissive, 2 });

			command->UniformData
			.SetInput("u_Material.DiffuseColor", mat.DiffuseColor);
			command->UniformData
			.SetInput("u_Material.SpecularColor", mat.SpecularColor);
			command->UniformData
			.SetInput("u_Material.EmissiveColor", mat.EmissiveColor);
		}

		command->AddIndices(mesh.Indices);
		command->AddVertices(mesh.Vertices);
	}

	if(!command->Calls || command->Calls[-1].InstanceCount >= 10'000) {
		auto& call = command->NewDrawCall();
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
		call.DepthTest = DepthTestingMode::On;
		call.Blending = BlendingMode::Greatest;
		call.Culling = CullingMode::Back;
		call.InstanceStart = s_InstancesIndex;
		s_InstancesIndex += 10'000;
	}

	auto* buffer = command->Pass->BufferData;
	auto& call = command->Calls[-1];
	RendererAPI::Get()
	->SetBufferData(buffer, DrawBufferIndex::Instances, glm::value_ptr(tr),
					1, call.InstanceStart + call.InstanceCount++);
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr,
						  DrawCommand* command)
{
	for(auto& subMesh : mesh->SubMeshes)
		DrawSubMesh(mesh, subMesh, tr, command);
}

void Renderer3D::DrawQuad(Ref<Quad> quad, const glm::mat4& tr,
						  DrawCommand* command)
{
	Ref<Mesh> mesh;
	if(quad->IsTextured)
		mesh =
			Mesh::Create(MeshType::Quad,
						 Material{ .Diffuse = quad->GetTexture() });
	else
		mesh = Mesh::Create(MeshType::Quad, quad->GetColor());

	DrawMesh(mesh, tr, command);
}

void Renderer3D::DrawQuad(Ref<Texture> texture, const glm::mat4& tr,
						  DrawCommand* command)
{
	DrawQuad(Quad::Create(texture), tr, command);
}

void Renderer3D::DrawQuad(const glm::vec4& color, const glm::mat4& tr,
						  DrawCommand* command)
{
	DrawQuad(Quad::Create(1, 1, color), tr, command);
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