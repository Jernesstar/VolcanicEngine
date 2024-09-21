#include "Renderer3D.h"

#include "Core/Assert.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace VolcaniCore {

void Renderer3D::Begin(Ref<Camera> camera) {
	auto& uniforms = Renderer::GetPass()->GetUniforms();

	uniforms
	.Set("u_ViewProj",
		[camera]() -> glm::mat4
		{
			return camera->GetViewProjection();
		});
	uniforms
	.Set("u_CameraPosition",
		[camera]() -> glm::vec3
		{
			return camera->GetPosition();
		});
}

void Renderer3D::End() {

}

void Renderer3D::DrawSkybox(Ref<Cubemap> cubemap) {
	RendererAPI::Get()->RenderCubemap(cubemap);
}

void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& tr) {
	if(!mesh)
		return;

	if(Renderer::GetDrawCommand().OptionsMap[DrawPrimitive::Mesh].Partition ==
		DrawPartition::Single)
	{
		Renderer::NewDrawCommand();
	}
	auto& command = Renderer::GetDrawCommand();
	auto& uniforms = command.GetUniforms();

	uniforms
	.Set("u_Diffuse",
		[mesh]() -> TextureSlot
		{
			Material& mat = mesh->GetMaterial();
			return { mat.Diffuse, 0 };
		});
	// uniforms
	// .Set("u_Specular",
	// 	[mesh]() -> TextureSlot
	// 	{
	// 		Material& mat = mesh->GetMaterial();
	// 		return { mat.Specular, 1 };
	// 	});

	command.AddMesh(mesh, tr);
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
	auto& command = Renderer::GetDrawCommand();

	// command.PointTransforms[point].Add(tr);
}

void Renderer3D::DrawLine(const Line& line, const glm::mat4& tr) {
	auto& command = Renderer::GetDrawCommand();

	// command.LineTransforms[line].Add(tr);
}

void Renderer3D::DrawText(Ref<Text> text, const glm::mat4& tr) {
	// TODO(Implement)
}

}