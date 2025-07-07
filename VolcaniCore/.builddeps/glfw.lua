project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/lib")

    files {
        "%{VendorPaths.glfw}/src/context.c",
        "%{VendorPaths.glfw}/src/init.c",
        "%{VendorPaths.glfw}/src/input.c",
        "%{VendorPaths.glfw}/src/monitor.c",
        "%{VendorPaths.glfw}/src/platform.c",
        "%{VendorPaths.glfw}/src/vulkan.c",
        "%{VendorPaths.glfw}/src/window.c",
        "%{VendorPaths.glfw}/src/null_init.c",
        "%{VendorPaths.glfw}/src/null_monitor.c",
        "%{VendorPaths.glfw}/src/null_window.c",
        "%{VendorPaths.glfw}/src/null_joystick.c",
        "%{VendorPaths.glfw}/src/egl_context.c",
        "%{VendorPaths.glfw}/src/osmesa_context.c"
    }

    includedirs {
        "%{VendorPaths.glfw}/src"
    }

    defines {
        "_GLFW_USE_HYBRID_HPG"
    }

    filter "system:windows"
        systemversion "latest"
        files {
            "%{VendorPaths.glfw}/src/win32_init.c",
            "%{VendorPaths.glfw}/src/win32_module.c",
            "%{VendorPaths.glfw}/src/win32_joystick.c",
            "%{VendorPaths.glfw}/src/win32_monitor.c",
            "%{VendorPaths.glfw}/src/win32_time.c",
            "%{VendorPaths.glfw}/src/win32_thread.c",
            "%{VendorPaths.glfw}/src/win32_window.c",
            "%{VendorPaths.glfw}/src/wgl_context.c"
        }

        defines {
            "_GLFW_WIN32"
        }

    filter "system:linux"
        files {
            "%{VendorPaths.glfw}/src/x11_init.c",
            "%{VendorPaths.glfw}/src/x11_monitor.c",
            "%{VendorPaths.glfw}/src/x11_window.c",
            "%{VendorPaths.glfw}/src/xkb_unicode.c",
            "%{VendorPaths.glfw}/src/posix_module.c",
            "%{VendorPaths.glfw}/src/posix_time.c",
            "%{VendorPaths.glfw}/src/posix_thread.c",
            "%{VendorPaths.glfw}/src/posix_poll.c",
            "%{VendorPaths.glfw}/src/linux_joystick.c",
            "%{VendorPaths.glfw}/src/glx_context.c"
        }

        defines {
            "_GLFW_X11"
        }
