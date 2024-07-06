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
        "TheMazeIsLava/src/**.cpp"
    }

    includedirs {
        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",
        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",
        "%{RootPath}",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",

        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}"
    }

    links {
        "Magma",
        "VolcaniCore",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",

        -- "bgfx",
        -- "bimg",
        -- "bx",
        "glfw",
        "glad",
        "assimp",
        "freetype",
        "stb_image"
    }

    filter "system:windows"
        systemversion "latest"
        links {
            "gdi32",
            "kernel32",
            "psapi"
        }
    filter "system:linux"
        links {
            "pthread",
            "dl",
            "GL",
            "X11"
        }

        buildoptions {
            "-fexceptions"
        }
    filter "system:macosx"
        links {
            "QuartzCore.framework",
            "Metal.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }
