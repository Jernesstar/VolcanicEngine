#include "StorageBuffer.h"

#include "Graphics/RendererAPI.h"

#include "OpenGL/StorageBuffer.h"

namespace VolcaniCore {

Ref<StorageBuffer> StorageBuffer::Create(const BufferLayout& layout,
										 uint64_t count, void* data)
{
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::StorageBuffer>(layout, count, data);
			break;
	}

	return nullptr;
}

}