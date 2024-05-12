project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/VolcaniCore/lib")

    files {
        "%{VendorPaths.imgui}/*.h",
        "%{VendorPaths.imgui}/*.cpp",

        "%{VendorPaths.imgui}/backends/imgui_impl_glfw.cpp",
        "%{VendorPaths.imgui}/backends/imgui_impl_opengl3.cpp",
    }

    includedirs {
        "%{Includes.glfw}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
    }
