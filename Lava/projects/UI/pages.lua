project "Home"
    kind "SharedLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("build/obj")
    targetdir ("build/lib/Home")

    files {
        "gen/Home.cpp"
    }

    includedirs {
        "gen",

        "%{ProjectSrcDir}",
        "%{ProjectSrcDir}/**",

        "%{VolcanicEngineDir}/VolcaniCore/src",
        "%{VolcanicEngineDir}/VolcaniCore/src/VolcaniCore",
        "%{VolcanicEngineDir}/VolcaniCore/src/impl",

        "%{VolcanicEngineDir}/Magma/src",
        "%{VolcanicEngineDir}/Magma/src/Magma",

        "%{VolcanicEngineDir}/Lava/src",
        "%{VolcanicEngineDir}/Lava/src/Lava",

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
        "%{VolcanicEngineDir}/build/Magma/lib",
        "%{VolcanicEngineDir}/build/Lava/lib"
    }

    links {
        "Project",

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
project "Level"
    kind "SharedLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("build/obj")
    targetdir ("build/lib/Level")

    files {
        "gen/Level.cpp"
    }

    includedirs {
        "gen",

        "%{ProjectSrcDir}",
        "%{ProjectSrcDir}/**",

        "%{VolcanicEngineDir}/VolcaniCore/src",
        "%{VolcanicEngineDir}/VolcaniCore/src/VolcaniCore",
        "%{VolcanicEngineDir}/VolcaniCore/src/impl",

        "%{VolcanicEngineDir}/Magma/src",
        "%{VolcanicEngineDir}/Magma/src/Magma",

        "%{VolcanicEngineDir}/Lava/src",
        "%{VolcanicEngineDir}/Lava/src/Lava",

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
        "%{VolcanicEngineDir}/build/Magma/lib",
        "%{VolcanicEngineDir}/build/Lava/lib"
    }

    links {
        "Project",

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
project "Pause"
    kind "SharedLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("build/obj")
    targetdir ("build/lib/Pause")

    files {
        "gen/Pause.cpp"
    }

    includedirs {
        "gen",

        "%{ProjectSrcDir}",
        "%{ProjectSrcDir}/**",

        "%{VolcanicEngineDir}/VolcaniCore/src",
        "%{VolcanicEngineDir}/VolcaniCore/src/VolcaniCore",
        "%{VolcanicEngineDir}/VolcaniCore/src/impl",

        "%{VolcanicEngineDir}/Magma/src",
        "%{VolcanicEngineDir}/Magma/src/Magma",

        "%{VolcanicEngineDir}/Lava/src",
        "%{VolcanicEngineDir}/Lava/src/Lava",

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
        "%{VolcanicEngineDir}/build/Magma/lib",
        "%{VolcanicEngineDir}/build/Lava/lib"
    }

    links {
        "Project",

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
