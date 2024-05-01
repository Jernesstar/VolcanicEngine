#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(VOLCANICENGINE_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#elif defined(VOLCANICENGINE_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif // VOLCANICENGINE_LINUX

#include <GLFW/glfw3native.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "Core/Defines.h"
#include "Core/Window.h"

namespace VolcaniCore::bgfx {

static void Init(Ref<VolcaniCore::Window> window) {
	::bgfx::PlatformData pd;
#if defined(VOLCANICENGINE_LINUX)
	pd.nwh = (void*)glfwGetX11Window(window->GetNativeWindow());
	pd.ndt = glfwGetX11Display();
#elif defined(VOLCANICENGINE_WINDOWS)
	pd.nwh = (void*)glfwGetWin32Window(window->GetNativeWindow());
#endif // VOLCANICENGINE_LINUX

	::bgfx::Init bgfxInit;
	bgfxInit.type = ::bgfx::RendererType::Count; // Automatically choose a renderer.
	bgfxInit.resolution.width = (uint16_t)window->GetWidth();
	bgfxInit.resolution.height = (uint16_t)window->GetHeight();
	bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
	bgfxInit.platformData = pd;
	::bgfx::init(bgfxInit);
}

}