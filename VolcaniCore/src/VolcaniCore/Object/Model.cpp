#include "Model.h"

#include "OpenGL/Model.h"

#include "Core/Application.h"
#include "Core/Assert.h"

namespace VolcaniCore {

Ref<VolcaniCore::Model> Model::Create(const std::string& path) {
	RendererBackend backend = RendererAPI::Get()->Backend;

	switch(backend) {
		case RenderAPI::OpenGL:
			return CreateRef<VolcaniCore::OpenGL::Model>(path);
			break;
	}
}

}