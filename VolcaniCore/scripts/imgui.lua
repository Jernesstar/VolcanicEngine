project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

    files {
        "%{VendorPaths.imgui}/*.h",
        "%{VendorPaths.imgui}/*.cpp",

        "%{VendorPaths.imgui}/backends/imgui_impl_glfw.cpp",
        "%{VendorPaths.imgui}/backends/imgui_impl_opengl3.cpp",
    }

    includedirs {
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.glfw}"
    }
