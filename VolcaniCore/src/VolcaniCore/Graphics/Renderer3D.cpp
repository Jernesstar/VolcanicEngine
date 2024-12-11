#include "Renderer3D.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

static DrawBuffer* s_CubemapBuffer;
static DrawBuffer* s_MeshBuffer;
// static DrawBuffer* s_PointBuffer;
static Map<Ref<Mesh>, std::pair<uint32_t, uint32_t>> s_Meshes;

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
			true // Structure of arrays
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
	DrawBufferSpecification specsCubemap{ cubemapLayout };

	s_MeshBuffer = RendererAPI::Get()->NewDrawBuffer(specs);
	s_CubemapBuffer =
		RendererAPI::Get()->NewDrawBuffer(specsCubemap, cubemapVertices);
}

void Renderer3D::Close() {
	RendererAPI::Get()->ReleaseBuffer(s_CubemapBuffer);
	RendererAPI::Get()->ReleaseBuffer(s_MeshBuffer);
}

void Renderer3D::Begin(Ref<Camera> camera) {
	s_Meshes.clear();

	Renderer::GetPass()->GetUniforms()
	.Set("u_ViewProj",
		[camera]() -> glm::mat4
		{
			return camera->GetViewProjection();
		})
	.Set("u_CameraPosition",
		[camera]() -> glm::vec3
		{
			return camera->GetPosition();
		});
}

void Renderer3D::End() {
	Renderer::EndCommand();
}

void Renderer3D::DrawSkybox(Ref<Cubemap> cubemap) {
	Renderer::GetCommand();

}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr) {
	if(!mesh)
		return;

	auto* command = Renderer::GetCommand();
	command->BufferData = s_MeshBuffer;

	auto& call = command->NewDrawCall();

	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Instanced;
	call.IndexCount  = mesh->GetIndices().size();
	call.VertexCount = mesh->GetVertices().size();
	call.InstanceCount = 1;

	if(!s_Meshes.count(mesh)) {
		s_Meshes[mesh] = { command->VerticesIndex, command->IndicesIndex };

		command->AddIndices(Buffer(mesh->GetIndices()));
		command->AddVertices(Buffer(mesh->GetVertices()));

		Renderer::GetPass()->GetUniforms()
		.Set("u_Diffuse",
			[mesh]() -> TextureSlot
			{
				Material& mat = mesh->GetMaterial();
				return { mat.Diffuse, 0 };
			})
		.Set("u_Specular",
			[mesh]() -> TextureSlot
			{
				Material& mat = mesh->GetMaterial();
				return { mat.Specular, 1 };
			});
	}
	else {
		call.VertexStart = s_Meshes[mesh].first;
		call.IndexStart = s_Meshes[mesh].second;
	}

	RendererAPI::Get()
		->SetBufferData(command->BufferData, 2, glm::value_ptr(tr), 1);
}

void Renderer3D::DrawModel(Ref<Model> model, const glm::mat4& tr) {
	for(auto& mesh : *model)
		DrawMesh(mesh, tr);
}

void Renderer3D::DrawQuad(Ref<Quad> quad, const glm::mat4& tr) {
	Ref<Mesh> mesh;
	if(quad->IsTextured)
		mesh = Mesh::Create(
				MeshPrimitive::Quad, Material{ .Diffuse = quad->GetTexture() });
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