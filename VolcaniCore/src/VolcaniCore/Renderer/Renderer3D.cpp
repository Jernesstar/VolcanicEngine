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
						  const glm::mat4& transform)
{
	auto pass = Renderer::GetPass();

	if(pass) {
		auto pipeline = pass->GetPipeline();
		Material& material = mesh->GetMaterial();

		pipeline->SetMat4("u_Model", transform);
		pipeline->SetTexture("u_Diffuse", material.Diffuse, 0);
		// pipeline->SetTexture("u_Specular",  material.Specular,  1);
		// pipeline->SetTexture("u_Roughness", material.Roughness, 2);
	}

	RendererAPI::Get()->DrawMesh(mesh, t);
}

void Renderer3D::DrawModel(Ref<Model> model,
						   const glm::mat4 transform)
{
	for(auto& mesh : *model)
		DrawMesh(mesh, t);
}

void Renderer3D::DrawQuad(Ref<Quad> quad,
						  const glm::mat4 transform)
{
	// TODO: Implement
}

void Renderer3D::DrawQuad(Ref<Texture> texture,
						  const glm::mat4 transform)
{
	DrawQuad(Quad::Create(texture), transform);
}

void Renderer3D::DrawQuad(const glm::vec4& color,
						  const glm::mat4 transform)
{
	DrawQuad(Quad::Create(color), transform);
}

void Renderer3D::DrawText(Ref<Text> text,
						  const glm::mat4 transform)
{
	// TODO: Implement
}

}