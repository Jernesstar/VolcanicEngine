#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>
#include <Renderer/Renderer.h>

#include <OpenGL/FrameBuffer.h>

using namespace VolcaniCore;

class FrameBufferDemo : public Application {
public:
	FrameBufferDemo();

	void OnUpdate(TimeStep ts);

private:
	Ref<FrameBuffer> m_FrameBuffer;
};

FrameBufferDemo::FrameBufferDemo() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	m_FrameBuffer = FrameBuffer::Create(800, 600);
}

void FrameBufferDemo::OnUpdate(TimeStep ts) {
	Renderer::Clear({ 0.34, 0.2, 0.87 });
}
