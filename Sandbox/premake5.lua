project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    exceptionhandling "On"
    rtti "Off"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Sandbox/obj")
    targetdir ("%{RootPath}/build/Sandbox/bin")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "src",

        "%{RootPath}",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        "%{RootPath}/Ash/src",
        "%{RootPath}/Ash/src/Ash",

        "%{RootPath}/Lava/src",
        "%{RootPath}/Lava/src/Lava",

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
        "%{Includes.rapidjson}",
        "%{Includes.PhysX}",
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
        "%{Includes.soloud}",
    }

    links {
        -- "Ash",
        "Lava",
        "Magma",
        "VolcaniCore",

        "glfw",
        "glad",
        "assimp",
        "freetype",
        "stb_image",

        "imgui",
        "yaml-cpp",
        "flecs",
        -- "rapidjson",
        "angelscript",
        "soloud",
    }

    filter "action:vs* or system:linux"
        links { "PhysX" }

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

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "toolset:msc"
        buildoptions {
            "/NODEFAULTLIB:library"
        }