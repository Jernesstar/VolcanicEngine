#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "Graphics/RendererAPI.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/Renderer3D.h"

namespace VolcaniCore {

const uint64_t Renderer::MaxTriangles = 1'000'000;
const uint64_t Renderer::MaxIndices   = 300'000; /* (3.0f / 2.0f) */
const uint64_t Renderer::MaxVertices  = 100'000;
const uint64_t Renderer::MaxInstances = MaxTriangles * 4;

static Ref<RenderPass> s_RenderPass;
static DrawCommand* s_DrawCommand;
static DrawCall s_Options;
static bool s_OptionsValid = false;
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

	Renderer2D::StartFrame();
	Renderer3D::StartFrame();
}

void Renderer::EndFrame() {
	s_Frame.Info.DrawCalls = DrawCallCount;
	s_Frame.Info.Indices   = IndexCount;
	s_Frame.Info.Vertices  = VertexCount;
	s_Frame.Info.Instances = InstanceCount;

	Renderer3D::EndFrame();
	Renderer2D::EndFrame();
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

	s_DrawCommand = RendererAPI::Get()->NewDrawCommand(buffer);
	s_DrawCommand->Pipeline = s_RenderPass->GetPipeline();
	s_DrawCommand->Image = s_RenderPass->GetOutput();
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

void Renderer::SetOptions(const DrawCall& options) {
	s_Options = options;
	s_OptionsValid = true;
}

DrawCall Renderer::GetOptions() {
	s_OptionsValid = false;
	return s_Options;
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