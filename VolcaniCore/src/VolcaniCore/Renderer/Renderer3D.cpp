#include "Renderer3D.h"

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

void Renderer3D::Init() {

}

void Renderer3D::Close() {

}

void Renderer3D::Begin(Ref<Camera> camera) {
	auto pass = Renderer::GetPass();
	if(!pass)
		return;

	auto pipeline = pass->GetPipeline();
	pipeline->SetMat4("u_ViewProj", camera->GetViewProjection());
	pipeline->SetVec3("u_CameraPosition", camera->GetPosition());
}

void Renderer3D::End() {

}

void Renderer3D::DrawCubemap(Ref<Cubemap> cubemap) {
	RendererAPI::Get()->DrawCubemap(cubemap);
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh,
						  const Transform& t)
{
	DrawMesh(mesh, t.GetTransform());
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh,
						  const glm::mat4& transform)
{
	auto pass = Renderer::GetPass();

	if(pass) {
		auto pipeline = pass->GetPipeline();
		Material& material = mesh->GetMaterial();

		pipeline->SetMat4("u_Model", transform);
		if(material.Diffuse)
			pipeline->SetTexture("u_Diffuse", material.Diffuse, 0);
		if(material.Specular)
			pipeline->SetTexture("u_Specular",  material.Specular, 1);
	}

	RendererAPI::Get()->DrawMesh(mesh, transform);
}

void Renderer3D::DrawModel(Ref<Model> model,
						   const Transform& t)
{
	for(auto& mesh : *model)
		DrawMesh(mesh, t);
}

void Renderer3D::DrawModel(Ref<Model> model,
						   const glm::mat4& transform)
{
	for(auto& mesh : *model)
		DrawMesh(mesh, transform);
}

void Renderer3D::DrawQuad(Ref<Quad> quad,
						  const Transform& t)
{
	DrawQuad(quad, t.GetTransform());
}

void Renderer3D::DrawQuad(Ref<Quad> quad,
						  const glm::mat4& transform)
{
	// TODO(Implement): Mesh::Create(MeshPrimitive, const std::vector<glm::vec4>& color_per_vertex)
	// DrawMesh(Mesh::Create(MeshPrimitive::Quad, quad->Color), transform);
}

void Renderer3D::DrawPoint(const Point& point,
						   const Transform& t)
{
	DrawPoint(point, t.GetTransform());
}

void Renderer3D::DrawPoint(const Point& point,
						   const glm::mat4& transform)
{
	// TODO(Implement)
}

void Renderer3D::DrawLine(const Line& line,
						  const Transform& t)
{
	DrawLine(line, t.GetTransform());
}

void Renderer3D::DrawLine(const Line& line,
						  const glm::mat4& transform)
{
	// TODO(Implement)
}

void Renderer3D::DrawQuad(Ref<Texture> texture,
						  const Transform& t)
{
	DrawQuad(Quad::Create(texture), t.GetTransform());
}

void Renderer3D::DrawQuad(Ref<Texture> texture,
						  const glm::mat4& transform)
{
	DrawQuad(Quad::Create(texture), transform);
}

void Renderer3D::DrawQuad(const glm::vec4& color,
						  const Transform& t)
{
	DrawQuad(Quad::Create(color), t.GetTransform());
}

void Renderer3D::DrawQuad(const glm::vec4& color,
						  const glm::mat4& transform)
{
	DrawQuad(Quad::Create(color), transform);
}

void Renderer3D::DrawText(Ref<Text> text,
						  const Transform& t)
{
	DrawText(text, t.GetTransform());
}

void Renderer3D::DrawText(Ref<Text> text,
						  const glm::mat4& transform)
{
	// TODO(Implement)
}

}