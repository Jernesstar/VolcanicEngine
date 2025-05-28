#include "StorageBuffer.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/StorageBuffer.h"

namespace VolcaniCore {

Ref<StorageBuffer> StorageBuffer::Create(const BufferLayout& layout,
										 uint64_t count)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::StorageBuffer>(layout, count);
			break;
	}

	return nullptr;
}

}