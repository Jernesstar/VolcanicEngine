#include "Renderer2D.h"

#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/OrthographicCamera.h"

#include "Object/Shader.h"

namespace VolcaniCore {

struct RendererData2D {

};

static RendererData2D s_Data;

void Renderer2D::Init() {

}

void Renderer2D::Close() {

}

void Renderer2D::Begin(Ref<OrthographicCamera> camera) {
	
}

void Renderer2D::End() {
	Flush();
}

void Renderer2D::DrawQuad(Ref<Quad> quad, Transform t) {

}

void Renderer2D::DrawQuad(const glm::vec4& color, Transform t) {
	// DrawQuad(CreateRef<Quad>(color)); // May be (or maybe) a bad idea
}

void Renderer2D::DrawQuad(Ref<Texture> texture, Transform t) {

}

void Renderer2D::DrawText(Ref<Text> text, Transform t) {

}

void Renderer2D::Flush() {

}

}