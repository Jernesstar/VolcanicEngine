#include "Shader.h"

#include "OpenGL/Shader.h"

#include "Core/Assert.h"
#include "Core/FileUtils.h"

#include "Graphics/RendererAPI.h"

namespace VolcaniCore {

Ref<ShaderPipeline> ShaderPipeline::Create(const List<Shader>& shaders) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
	}

	return nullptr;
}

}