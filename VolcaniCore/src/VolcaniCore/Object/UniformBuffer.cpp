#include "Cubemap.h"

#include "RendererAPI.h"

#include "OpenGL/UniformBuffer.h"

namespace VolcaniCore {


Ref<Cubemap> UniformBuffer::Create() {
	RendererBackend api = RendererAPI::Get()->Backend;

	switch(backend) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::UniformBuffer>();
			break;
	}
}


}