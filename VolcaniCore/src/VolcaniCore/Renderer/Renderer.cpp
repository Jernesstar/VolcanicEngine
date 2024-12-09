#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"

namespace VolcaniCore {

const uint64_t Renderer::MaxTriangles = 1'000'000;
const uint64_t Renderer::MaxIndices   = MaxVertices  * 3; /* (3.0f / 2.0f) */
const uint64_t Renderer::MaxVertices  = MaxTriangles * 3;
const uint64_t Renderer::MaxInstances = 200'000;

static Ref<RenderPass> s_RenderPass;
static DrawCommand* s_DrawCommand;
static FrameData s_Frame;

static uint64_t DrawCallCount;
static uint64_t IndexCount;
static uint64_t VertexCount;
static uint64_t InstanceCount;

void Renderer::Init() {
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
	InstanceCount  = 0;
}

void Renderer::EndFrame() {
	s_Frame.Info.DrawCalls = DrawCallCount;
	s_Frame.Info.Indices   = IndexCount;
	s_Frame.Info.Vertices  = VertexCount;
	s_Frame.Info.Instances = InstanceCount;
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	s_RenderPass = pass;
	NewCommand();
}

void Renderer::EndPass() {
	EndCommand();
	s_RenderPass = nullptr;
}

Ref<RenderPass> Renderer::GetPass() {
	return s_RenderPass;
}

DrawCommand* Renderer::GetCommand() {
	return s_DrawCommand;
}

DrawCommand* Renderer::NewCommand(DrawBuffer* buffer) {
	EndCommand();

	s_DrawCommand = RendererAPI::Get()->NewDrawCommand();
	s_DrawCommand->Pipeline = s_RenderPass->GetPipeline();
	s_DrawCommand->Image = s_RenderPass->GetOutput();
	s_DrawCommand->BufferData = buffer;
	return s_DrawCommand;
}

void Renderer::EndCommand() {
	if(!s_DrawCommand)
		return;

	DrawCallCount += s_DrawCommand->Calls.size();
	IndexCount    += s_DrawCommand->IndicesIndex;
	VertexCount   += s_DrawCommand->VerticesIndex;
	InstanceCount += s_DrawCommand->InstancesIndex;

	s_RenderPass->SetUniforms(s_DrawCommand->UniformData);
	s_DrawCommand = nullptr;
}

void Renderer::Clear() {
	VOLCANICORE_ASSERT(s_DrawCommand, "Did you forget to call StartPass?");
	s_DrawCommand->Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	VOLCANICORE_ASSERT(s_DrawCommand, "Did you forget to call StartPass?");
	s_DrawCommand->ViewportWidth = width;
	s_DrawCommand->ViewportHeight = height;
}

void Renderer::Flush() {

}

FrameDebugInfo Renderer::GetDebugInfo() {
	return s_Frame.Info;
}

FrameData& Renderer::GetFrame() {
	return s_Frame;
}

}