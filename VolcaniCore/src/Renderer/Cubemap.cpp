#pragma once

#include "Application.h"

#include "OpenGL/Cubemap.h"

namespace VolcaniCore {

Ref<Cubmap> Cubemap::Create(const std::string& cubemap_folder) {
	RenderAPI api = Application::Get()->GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<

	}
}

Ref<Cubemap> Cubemap::Create(const std::vector<std::string>& faces) {


}

}