project "Lava"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Lava/obj")
    targetdir ("%{RootPath}/build/Lava/lib")

    files {
        "src/Lava/**.h",
        "src/Lava/**.cpp"
    }

    includedirs {
        "src/Lava",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.flecs}",
        "%{Includes.rapidjson}",
        "%{Includes.PhysX}",

        "%{Includes.glm}",
        "%{Includes.glad}",
        "%{Includes.glfw}",
        "%{Includes.imgui}",
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
    }

    links {
        "Magma",
        "VolcaniCore",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",
        "flecs",
        -- "rapidjson",
        "angelscript",
    }

    filter "action:vs* or system:linux"
        links { "PhysX" }

    filter "system:linux"
        buildoptions {
            "-fPIC",
            "-Wl,-Bsymbolic"
        }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "system:windows"
        systemversion "latest"


project "Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Lava/obj")
    targetdir ("%{RootPath}/build/Lava/bin")

    files {
        "src/Runtime/**.cpp"
    }

    includedirs {
        "src",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        "%{Includes.glm}",
        "%{Includes.glad}",
        "%{Includes.glfw}",
        "%{Includes.imgui}",

        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.flecs}",
        "%{Includes.rapidjson}",
        "%{Includes.PhysX}",
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
    }

    links {
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
        -- "rapidjson",
        "angelscript",
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

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }
