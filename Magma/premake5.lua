project "Magma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "src/Magma/**.h",
        "src/Magma/**.cpp"
    }

    includedirs {
        "src/Magma",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        "%{Includes.glm}",
        "%{Includes.glad}",
        "%{Includes.glfw}",

        "%{Includes.yaml_cpp}",
        "%{Includes.rapidjson}",

        "%{Includes.imgui}/imgui",
        "%{Includes.flecs}",
        "%{Includes.PhysX}",

        "%{Includes.imgui}",
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
        "%{Includes.soloud}",
    }

    links {
        "VolcaniCore",

        "yaml_cpp",
        -- "rapidjson",

        "imgui",
        "ImGuiFileDialog",
        "flecs",
        "angelscript",
        "soloud"
    }

    filter "action:vs* or system:linux"
        links { "PhysX" }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "system:windows"
        systemversion "latest"


include "Magma/.builddeps/imgui"
include "Magma/.builddeps/flecs"
include "Magma/.builddeps/angelscript"
include "Magma/.builddeps/PhysX"
include "Magma/.builddeps/soloud"


project "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/bin")

    files {
        "src/Editor/**.h",
        "src/Editor/**.cpp",
    }

    includedirs {
        "src/Editor",

        "src/Magma",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

        "%{RootPath}/Magma/src",
        "%{RootPath}/Magma/src/Magma",

        "%{RootPath}/Lava/src",

        "%{Includes.glm}",
        "%{Includes.glad}",
        "%{Includes.glfw}",

        "%{Includes.assimp}",
        "%{Includes.stb_image}",
        "%{Includes.freetype}",

        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.rapidjson}",

        "%{Includes.flecs}",
        "%{Includes.rapidjson}",
        "%{Includes.PhysX}",

        "%{Includes.ImGuiFileDialog}",

        "%{Includes.imgui}",
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
        "%{Includes.soloud}",
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

        "ImGuiFileDialog",
        "imgui",
        "yaml-cpp",
        -- "rapidjson",
        "flecs",
        "angelscript",
        "soloud"
    }

    filter "toolset:msc or system:linux"
        links "PhysX"

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

include "Magma/.builddeps/ImGuiFileDialog"
include "Magma/.builddeps/assimp"
include "Magma/.builddeps/rapidjson"
include "Magma/.builddeps/yaml-cpp"
include "Magma/.builddeps/freetype"
include "Magma/.builddeps/stb_image"