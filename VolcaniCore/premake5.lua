project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "On"

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

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}"
    }

    links {
        -- "bgfx",
        -- "bimg",
        -- "bx",
        "glfw",
        "glad",
        "assimp",
        "freetype",
        "stb_image"
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-Wno-format-security"
        }

    filter "system:windows"
        systemversion "latest"


-- include "VolcaniCore/scripts/bgfx"
include "VolcaniCore/scripts/glfw"
include "VolcaniCore/scripts/glad"
include "VolcaniCore/scripts/assimp"
include "VolcaniCore/scripts/freetype"
include "VolcaniCore/scripts/stb_image"
