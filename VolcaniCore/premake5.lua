project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/VolcaniCore/lib")

    files {
        "src/VolcaniCore/**.h",
        "src/VolcaniCore/**.cpp",
        "src/impl/**.h",
        "src/impl/**.cpp"
    }

    includedirs {
        "src/",
        "src/VolcaniCore",
        "src/impl",

        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}"
    }

    links {
        "glfw",
        "glad",
        "assimp",
        "freetype",
        "stb_image"
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "system:windows"
        systemversion "latest"


include "VolcaniCore/.builddeps/glfw"
include "VolcaniCore/.builddeps/glad"
include "VolcaniCore/.builddeps/assimp"
include "VolcaniCore/.builddeps/freetype"
include "VolcaniCore/.builddeps/stb_image"
