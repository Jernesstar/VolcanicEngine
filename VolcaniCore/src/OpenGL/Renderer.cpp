#include "Renderer.h"

namespace VolcaniCore::OpenGL {

void Renderer::Init() {

}

void Renderer::Close() {

}

void Renderer::Clear() {

}

void Renderer::RenderMesh(Ref<Mesh> mesh) {

}

void Renderer::RenderCubemap(Ref<Cubemap> cubemap) {

}

void Renderer::RenderQuad(Ref<Quad> quad, Transform t) {

}

void Renderer::RenderText(Ref<Text> text, Transform t) {

}

void Renderer::RenderTexture(Ref<Texture> texture, Transform t) {

}

void Renderer::DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices)
{
    vertex_array->Bind();
    glDrawElements(GL_TRIANGLES, indices != 0 ? indices : vertex_array->GetIndexBuffer()->Count,
        GL_UNSIGNED_INT, nullptr);
}

}