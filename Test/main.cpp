#include <iostream>
#include <memory>

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer.h>

using namespace VolcaniCore;

struct BackendBuffer {
	// Ref<VertexArray> Array;
	Buffer<uint32_t> Indices;
	Buffer<void> Vertices;
	Buffer<void> Instances;
};

struct RendererData {
	Map<DrawBuffer*, BackendBuffer> Arrays;
	List<DrawCommand> Commands;
	List<DrawPass> Passes;
	DrawPass* LastPass = nullptr;
};

static RendererData s_Data;

void func() {
	s_Data.Commands.Allocate(50);
}

void func2() {
	DrawPass pass;

}

int main() {
	std::cout << "Begin\n";
	func();
	func2();
	func2();
	std::cout << "End\n";

	return 0;
}