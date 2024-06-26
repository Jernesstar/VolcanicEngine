#include "FrameBuffer.h"

#include "OpenGL/FrameBuffer.h"

#include "Core/Application.h"

namespace VolcaniCore {

Ref<FrameBuffer> FrameBuffer::Create(uint32_t width, uint32_t height) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::FrameBuffer>(
							OpenGL::AttachmentSpecification(width, height));
			break;
	}
}

}