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

void Renderer3D::DrawMesh(Ref<Mesh> mesh, Transform t) {
	// auto pass = Renderer::GetPass();
	// auto pipeline = pass->GetPipeline();
	// Material& material = mesh->GetMaterial();

	// pipeline->SetTexture("u_Diffuse", material.Diffuse, 0);
	// pipeline->SetMat4("u_Model", t.GetTransform());
	// pipeline->SetTexture("u_Specular",  material.Specular,  1);
	// pipeline->SetTexture("u_Roughness", material.Roughness, 2);

	RendererAPI::Get()->DrawMesh(mesh, t);
}

void Renderer3D::DrawModel(Ref<Model> model, Transform t) {
	for(auto& mesh : *model)
		DrawMesh(mesh, t);
}

void Renderer3D::DrawQuad(Ref<Quad> quad, Transform t) {

}

void Renderer3D::DrawQuad(const glm::vec4& color, Transform t) {

}

void Renderer3D::DrawQuad(Ref<Texture> texture, Transform t) {

}

void Renderer3D::DrawText(Ref<Text> text, Transform t) {

}

}