#include "Model.h"

#include "OpenGL/Model.h"

#include "Core/Application.h"
#include "Core/Assert.h"

namespace VolcaniCore {

Ref<VolcaniCore::Model> Model::Create(const std::string& path) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<VolcaniCore::OpenGL::Model>(path);
			break;
	}
}

}