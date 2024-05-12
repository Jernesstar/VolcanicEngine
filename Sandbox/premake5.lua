project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    exceptionhandling "Off"
    rtti "Off"

    objdir ("%{RootPath}/build/Sanbox/obj")
    targetdir ("%{RootPath}/build/Sanbox/bin")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",
        "%{RootPath}/Magma/src",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.freetype}",
        "%{Includes.yaml_cpp}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}"
    }

    links {
        "VolcaniCore",
        "Magma",
        -- "bgfx",
        -- "bimg",
        -- "bx",
        "glfw",
        "glad",
        "imgui",
        "assimp",
        "yaml-cpp",
        "freetype",
        "stb_image",
    }

    filter "system:windows"
        systemversion "latest"
        links {
            "gdi32",
            -- "kernel32",
            -- "psapi"
        }
    filter "system:linux"
        links {
            "pthread",
            "dl",
            "GL",
            "X11"
        }
    filter "system:macosx"
        links {
            "QuartzCore.framework",
            "Metal.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }
