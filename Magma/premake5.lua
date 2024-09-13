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
        "%{RootPath}/VolcaniCore/src/impl/",

        "%{Includes.imgui}/imgui",
        "%{Includes.yaml_cpp}",
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.flecs}",
        "%{Includes.PhysX}",

        "%{Includes.glm}",
        "%{Includes.glad}",
        "%{Includes.glfw}",
        "%{Includes.imgui}",
    }

    links {
        "VolcaniCore",

        "imgui",
        "yaml-cpp",
        "ImGuiFileDialog",
        "flecs",
        "PhysX",
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security"
        }

    filter "system:windows"
        systemversion "latest"


include "Magma/scripts/imgui"
include "Magma/scripts/yaml-cpp"
include "Magma/scripts/ImGuiFileDialog"
include "Magma/scripts/flecs"
include "Magma/scripts/PhysX"
