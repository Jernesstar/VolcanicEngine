project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    exceptionhandling "Off"
    rtti "Off"
    staticruntime "On"

    objdir ("%{RootPath}/build/Sanbox/obj")
    targetdir ("%{RootPath}/build/Sanbox/bin")

    files {
        "src/**.h",
        "src/**.cpp",
        "TheMazeIsLava/src/**.cpp"
    }

    includedirs {
        "%{RootPath}",
        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",
        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        -- "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",

        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.flecs}",
        "%{Includes.PhysX}"
    }

    links {
        "Magma",
        "VolcaniCore",

        "flecs",
    }

    filter "system:linux"
        links {
            "pthread",
            "dl",
            "GL",
            "X11",
        }

    filter "system:windows"
        systemversion "latest"
        links {
            "gdi32",
            "kernel32",
            "psapi",
            "Ws2_32",
        }
    filter "system:macosx"
        links {
            "QuartzCore.framework",
            "Metal.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security"
        }