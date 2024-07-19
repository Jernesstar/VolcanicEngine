workspace "VolcanicEngine"
    location ("build")
    architecture "x86_64"
    configurations { "Debug", "Release" }

    filter "system:linux"
        defines "VOLCANICENGINE_LINUX"

    filter "system:windows"
        defines {
            "VOLCANICENGINE_WINDOWS",
        }
    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"

    filter "action:vs*"
        startproject "Sandbox"

include "VolcaniCore"
include "Magma"
include "Sandbox"

RootPath = _MAIN_SCRIPT_DIR;
VolcaniCoreVendorDir = "%{RootPath}/VolcaniCore/vendor"
MagmaVendorDir = "%{RootPath}/Magma/vendor"

VendorPaths = {}
Includes = {}

VendorPaths["bgfx"]      = "%{VolcaniCoreVendorDir}/bgfx"
VendorPaths["bimg"]      = "%{VolcaniCoreVendorDir}/bimg"
VendorPaths["bx"]        = "%{VolcaniCoreVendorDir}/bx"
VendorPaths["glm"]       = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"]      = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"]      = "%{VolcaniCoreVendorDir}/glfw"
VendorPaths["assimp"]    = "%{VolcaniCoreVendorDir}/assimp"
VendorPaths["freetype"]  = "%{VolcaniCoreVendorDir}/freetype"
VendorPaths["stb_image"] = "%{VolcaniCoreVendorDir}/stb_image"

VendorPaths["imgui"]           = "%{MagmaVendorDir}/imgui"
VendorPaths["yaml_cpp"]        = "%{MagmaVendorDir}/yaml-cpp"
VendorPaths["ImGuiFileDialog"] = "%{MagmaVendorDir}/ImGuiFileDialog"
VendorPaths["flecs"]           = "%{MagmaVendorDir}/flecs"
VendorPaths["PhysX"]           = "%{MagmaVendorDir}/PhysX"

Includes["bgfx"]      = "%{VendorPaths.bgfx}/include"
Includes["bimg"]      = "%{VendorPaths.bimg}/include"
Includes["bx"]        = "%{VendorPaths.bx}/include"
Includes["glm"]       = "%{VendorPaths.glm}"
Includes["glad"]      = "%{VendorPaths.glad}/include"
Includes["glfw"]      = "%{VendorPaths.glfw}/include"
Includes["assimp"]    = "%{VendorPaths.assimp}/include"
Includes["freetype"]  = "%{VendorPaths.freetype}/include"
Includes["stb_image"] = "%{VendorPaths.stb_image}/include"

Includes["imgui"]           = "%{MagmaVendorDir}"
Includes["yaml_cpp"]        = "%{VendorPaths.yaml_cpp}/include"
Includes["ImGuiFileDialog"] = "%{MagmaVendorDir}"
Includes["flecs"]           = "%{VendorPaths.flecs}/include"
Includes["PhysX"]           = "%{VendorPaths.PhysX}/physx/include"
