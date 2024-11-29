#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"

namespace VolcaniCore {

// static void FlushDrawCalls();

const uint64_t Renderer::MaxTriangles = 1'000'000;
const uint64_t Renderer::MaxIndices   = MaxVertices  * 3; /* (3.0f / 2.0f) */
const uint64_t Renderer::MaxVertices  = MaxTriangles * 3;
const uint64_t Renderer::MaxInstances = 200'000;

static Ref<RenderPass> s_RenderPass;
static DrawCall* s_DrawCall;
static FrameData s_Frame;

static uint64_t DrawCallCount;
static uint64_t IndexCount;
static uint64_t VertexCount;
static uint64_t TransformCount;

void Renderer::Init()
{
	s_Frame = { };

	Renderer2D::Init();
	Renderer3D::Init();
}

void Renderer::Close() {
	Renderer3D::Close();
	Renderer2D::Close();
}

void Renderer::BeginFrame() {
	DrawCallCount  = 0;
	IndexCount     = 0;
	VertexCount    = 0;
	TransformCount = 0;
}

void Renderer::EndFrame() {
	Flush();

	s_Frame.Info.DrawCalls = DrawCallCount;
	s_Frame.Info.Indices   = IndexCount;
	s_Frame.Info.Vertices  = VertexCount;
	s_Frame.Info.Instances = TransformCount;

	// s_DrawCommand->Clear();
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	
}

void Renderer::EndPass() {

}

Ref<RenderPass> Renderer::GetPass() {
	// return s_RenderPass;
}

FrameDebugInfo Renderer::GetDebugInfo() {
	return s_Frame.Info;
}

FrameData& Renderer::GetFrame() {
	return s_Frame;
}

}