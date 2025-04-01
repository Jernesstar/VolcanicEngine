#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Defines.h"

#include "RendererAPI.h"
#include "RenderPass.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

namespace VolcaniCore {

const uint64_t Renderer::MaxTriangles = 1'000'000;
const uint64_t Renderer::MaxIndices   = MaxTriangles * 6;
const uint64_t Renderer::MaxVertices  = MaxTriangles * 3;
const uint64_t Renderer::MaxInstances = MaxTriangles * 4;

static FrameData s_Frame;
static Ref<RenderPass> s_RenderPass;
static List<DrawCommand*> s_Stack;

static DrawCall s_Options;
static bool s_OptionsValid = false;

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
	Renderer2D::StartFrame();
	Renderer3D::StartFrame();
}

void Renderer::EndFrame() {
	auto info = RendererAPI::Get()->GetDebugInfo();
	s_Frame.Info.DrawCalls = info.DrawCallCount;
	s_Frame.Info.Indices   = info.IndexCount;
	s_Frame.Info.Vertices  = info.VertexCount;
	s_Frame.Info.Instances = info.InstanceCount;

	Renderer3D::EndFrame();
	Renderer2D::EndFrame();
}

void Renderer::StartPass(Ref<RenderPass> pass, bool pushCommand) {
	s_RenderPass = pass;
	if(pushCommand)
		PushCommand();
}

void Renderer::EndPass() {
	s_Stack.Clear();
	s_RenderPass = nullptr;
}

Ref<RenderPass> Renderer::GetPass() {
	return s_RenderPass;
}

DrawCommand* Renderer::PushCommand() {
	s_Stack.Add(NewCommand());
	return GetCommand();
}

void Renderer::PopCommand() {
	if(!s_Stack)
		return;

	s_RenderPass->SetUniforms(s_Stack[-1]);
	s_Stack.Pop();
}

DrawCommand* Renderer::GetCommand() {
	return s_Stack[-1];
}

DrawCommand* Renderer::NewCommand(bool usePrevious) {
	if(usePrevious && s_Stack && !s_Stack[-1]->Calls)
		return s_Stack[-1];

	return RendererAPI::Get()->NewDrawCommand(s_RenderPass->Get());
}

void Renderer::Clear() {
	if(!s_Stack) {
		RendererAPI::Get()->NewDrawCommand(nullptr)->Clear = true;
		Renderer::Flush();
	}
	else
		GetCommand()->Clear = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	GetCommand()->ViewportWidth = width;
	GetCommand()->ViewportHeight = height;
}

void Renderer::PushOptions(const DrawCall& options) {
	s_Options = options;
	s_OptionsValid = true;
}

void Renderer::PopOptions(uint32_t count) {
	s_OptionsValid = false;
}

void Renderer::Flush() {
	s_Stack.Clear();
	RendererAPI::Get()->EndFrame();
}

FrameDebugInfo Renderer::GetDebugInfo() {
	return s_Frame.Info;
}

FrameData& Renderer::GetFrame() {
	return s_Frame;
}

}