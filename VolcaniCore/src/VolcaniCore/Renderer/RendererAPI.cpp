#include "RendererAPI.h"

#include "OpenGL/Renderer.h"

namespace VolcaniCore {

Ref<RendererAPI> RendererAPI::CreateRenderer(RenderAPI api) {
	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::Renderer>();
	}

	return CreateRef<OpenGL::Renderer>();
}

}