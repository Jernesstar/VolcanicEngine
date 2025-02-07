#include "UniformBuffer.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/UniformBuffer.h"

namespace VolcaniCore {

Ref<UniformBuffer> UniformBuffer::Create(const BufferLayout& layout,
										 uint64_t count)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::UniformBuffer>(layout, count);
			break;
	}

	return nullptr;
}

}