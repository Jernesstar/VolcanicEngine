project "Magma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

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

        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.flecs}",
        "%{Includes.PhysX}",

        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
        "%{Includes.soloud}",
    }

    links {
        "VolcaniCore",

        "yaml-cpp",

        "imgui",
        "flecs",
        "angelscript",
        "soloud"
    }

    defines {
        "flecs_STATIC"
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
include "Magma/.builddeps/yaml-cpp"

project "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/bin")

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

        "%{Includes.efsw}",

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
        "efsw",

        "imgui",
        "ImGuiFileDialog",
        "ImGuizmo",

        "yaml-cpp",
        "flecs",
        "angelscript",
        "soloud"
    }

    defines {
        "flecs_STATIC"
    }

    filter "toolset:msc or system:linux"
        links "PhysX"
        debugdir ".."

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
include "Magma/.builddeps/ImGuizmo"
include "Magma/.builddeps/assimp"
include "Magma/.builddeps/freetype"
include "Magma/.builddeps/stb_image"
include "Magma/.builddeps/efsw"
