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
    }

    links {
        "Magma",
        -- "VolcaniCore",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",
        "flecs",
        "rapidjson",
        -- "PhysX",
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
    targetdir ("%{RootPath}/build/Lava/lib")

    files {
        "main.cpp"
    }

    includedirs {
        "src",

        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/src/VolcaniCore",
        "%{RootPath}/VolcaniCore/src/impl",

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
    }

    links {
        "Lava",
        "Magma",
        "VolcaniCore",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",
        "flecs",
        "rapidjson",
        -- "PhysX",
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "system:windows"
        systemversion "latest"

include "Magma/.builddeps/imgui"
include "Magma/.builddeps/yaml-cpp"
include "Magma/.builddeps/ImGuiFileDialog"
include "Magma/.builddeps/flecs"
include "Magma/.builddeps/rapidjson"
-- include "Magma/.builddeps/PhysX"
