#include "Renderer2D.h"

#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/OrthographicCamera.h"

#include "Object/Shader.h"

namespace VolcaniCore {

void Renderer2D::Begin(Ref<OrthographicCamera> camera) {
	
}

void Renderer2D::End() {

}

void Renderer2D::DrawQuad(Ref<Quad> quad, const Transform& t) {

}

void Renderer2D::DrawQuad(const glm::vec4& color, const Transform& t) {
	DrawQuad(Quad::Create(1, 1, color), t);
}

void Renderer2D::DrawQuad(Ref<Texture> texture, const Transform& t) {
	DrawQuad(Quad::Create(texture), t);
}

void Renderer2D::DrawText(Ref<Text> text, const Transform& t) {

}

void Renderer2D::DrawFullScreenQuad(Ref<Framebuffer> framebuffer,
									AttachmentTarget target)
{

}

}