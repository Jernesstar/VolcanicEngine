#include "UniformBuffer.h"

#include "Renderer/RendererAPI.h"

#include "OpenGL/UniformBuffer.h"

namespace VolcaniCore {

// Ref<UniformBuffer> UniformBuffer::Create(const std::string& name,
// 										 const BufferLayout& layout)
// {
// 	RendererAPI::Backend backend = RendererAPI::GetBackend();

// 	switch(backend) {
// 		case RendererAPI::Backend::OpenGL:
// 			return CreateRef<OpenGL::UniformBuffer>(name, layout);
// 			break;
// 	}
// }

}