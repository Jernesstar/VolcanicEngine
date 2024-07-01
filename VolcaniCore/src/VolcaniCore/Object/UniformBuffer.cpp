#include "Cubemap.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/UniformBuffer.h"

namespace VolcaniCore {


Ref<UniformBuffer> UniformBuffer::Create() {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RendererBackend::OpenGL:
			return CreateRef<OpenGL::UniformBuffer>();
			break;
	}
}


}