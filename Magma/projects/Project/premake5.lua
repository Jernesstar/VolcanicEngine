project "Project"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    -- exceptionhandling "On"
    -- rtti "Off"
    -- staticruntime "Off"

    -- ProjectSrcDir = _OPTIONS["src"]
    ProjectSrcDir = "C:/Users/Jercy Mukala/Code/TheMazeIsLava/TheMazeIsLava/src"

    objdir ("build/obj")
    targetdir ("build/bin")

    files {
        "%{ProjectSrcDir}/**.h",
        "%{ProjectSrcDir}/**.cpp",
        "gen/*.cpp"
    }
    print(ProjectSrcDir)

    includedirs {
        "%{ProjectSrcDir}/**",

        "%{VolcanicEngineDir}/VolcaniCore/src",
        "%{VolcanicEngineDir}/VolcaniCore/src/VolcaniCore",
        "%{VolcanicEngineDir}/VolcaniCore/src/impl",
        "%{VolcanicEngineDir}/Magma/src",
        "%{VolcanicEngineDir}/Magma/src/Magma",
        "%{VolcanicEngineDir}/Ash/src",
        "%{VolcanicEngineDir}/Ash/src/Magma",

        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.flecs}",
        "%{Includes.rapidjson}",
        "%{Includes.PhysX}",

        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",
    }

    libdirs {
        "%{VolcanicEngineDir}/build/VolcaniCore/lib",
        "%{VolcanicEngineDir}/build/Magma/lib"
    }

    links {
        "Magma",
        "VolcaniCore",

        "glfw",
        "glad",
        "assimp",
        "freetype",
        "stb_image",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",
        "flecs",
        "rapidjson",
        -- "PhysX",
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

    filter "toolset:msc"
        buildoptions {
            "/NODEFAULTLIB:library"
        }