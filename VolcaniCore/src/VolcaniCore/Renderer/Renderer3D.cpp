#include "Renderer3D.h"

#include "Core/Assert.h"

#include "RendererAPI.h"

namespace VolcaniCore {

struct RendererData3D {
	Ref<ShaderPipeline> MeshShader;

	glm::mat4 ViewProjection;
	glm::vec3 CameraPosition;
};

static RendererData3D s_Data;

void Renderer3D::Init() {
	// s_Data.MeshShader = ShaderPipeline::Create({
	// 	{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
	// 	{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	// });
}

void Renderer3D::Close() {

}

void Renderer3D::Begin(Ref<Camera> camera) {
	s_Data.ViewProjection = camera->GetViewProjection();
	s_Data.CameraPosition = camera->GetPosition();

	// s_Data.MeshShader->Bind();
	// s_Data.MeshShader->SetVec3("u_CameraPosition", s_Data.CameraPosition);
	// s_Data.MeshShader->SetMat4("u_ViewProj", s_Data.ViewProjection);
}

void Renderer3D::End() {

}

void Renderer3D::DrawCubemap(Ref<Cubemap> cubemap) {

}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, Transform t) {

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