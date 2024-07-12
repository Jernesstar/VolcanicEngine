#include "Renderer3D.h"

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

struct RendererData3D {
	glm::mat4 ViewProjection;
	glm::vec3 CameraPosition;

	std::unordered_map<Ref<VolcaniCore::Mesh>, uint32_t> Meshes;
};

static RendererData3D s_Data;

void Renderer3D::Init() {

}

void Renderer3D::Close() {

}

void Renderer3D::Begin(Ref<Camera> camera) {
	s_Data.ViewProjection = camera->GetViewProjection();
	s_Data.CameraPosition = camera->GetPosition();

	auto pass = Renderer::GetPass();
	if(!pass)
		return;

	auto pipeline = pass->GetPipeline();
	pipeline->SetVec3("u_CameraPosition", s_Data.CameraPosition);
	pipeline->SetMat4("u_ViewProj", s_Data.ViewProjection);
}

void Renderer3D::End() {
	auto pass = Renderer::GetPass();

	for(auto& [mesh, instanceCount] : s_Data.Meshes) {
		if(pass) {
			auto pipeline = pass->GetPipeline();
			Material& material = mesh->GetMaterial();

			pipeline->SetTexture("u_Diffuse", material.Diffuse, 0);
			// pipeline->SetTexture("u_Specular",  material.Specular,  1);
			// pipeline->SetTexture("u_Roughness", material.Roughness, 2);
		}

		RendererAPI::Get()->DrawInstanced(mesh);
	}

	s_Data.Meshes.clear();
}

void Renderer3D::DrawCubemap(Ref<Cubemap> cubemap) {
	RendererAPI::Get()->DrawCubemap(cubemap);
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, Transform t) {
	s_Data.Meshes.emplace(mesh, 0).first->second++;

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