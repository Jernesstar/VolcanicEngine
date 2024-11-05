workspace "Projects"
    location ("build")
    architecture "x86_64"
    configurations { "Debug", "Release" }

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"

    newoption {
        trigger = "project",
        value = "Path",
        description = "The path of a directory containing a .volc.proj file"
    }

    newoption {
        trigger = "src",
        value = "SrcPath",
        description = "The path to the directory of a the source files"
    }

include "Project/premake5.lua"

VolcanicEngineDir    = "%{_MAIN_SCRIPT_DIR}/../../";
VolcaniCoreVendorDir = "%{VolcanicEngineDir}/VolcaniCore/vendor"
MagmaVendorDir       = "%{VolcanicEngineDir}/Magma/vendor"

VendorPaths = {}
Includes = {}

VendorPaths["glm"]             = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"]            = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"]            = "%{VolcaniCoreVendorDir}/glfw"
VendorPaths["assimp"]          = "%{VolcaniCoreVendorDir}/assimp"
VendorPaths["freetype"]        = "%{VolcaniCoreVendorDir}/freetype"
VendorPaths["stb_image"]       = "%{VolcaniCoreVendorDir}/stb_image"

VendorPaths["imgui"]           = "%{MagmaVendorDir}/imgui"
VendorPaths["yaml_cpp"]        = "%{MagmaVendorDir}/yaml-cpp"
VendorPaths["ImGuiFileDialog"] = "%{MagmaVendorDir}/ImGuiFileDialog"
VendorPaths["flecs"]           = "%{MagmaVendorDir}/flecs"
VendorPaths["rapidjson"]       = "%{MagmaVendorDir}/rapidjson"
VendorPaths["PhysX"]           = "%{MagmaVendorDir}/PhysX"

Includes["glm"]                = "%{VendorPaths.glm}"
Includes["glad"]               = "%{VendorPaths.glad}/include"
Includes["glfw"]               = "%{VendorPaths.glfw}/include"
Includes["assimp"]             = "%{VendorPaths.assimp}/include"
Includes["freetype"]           = "%{VendorPaths.freetype}/include"
Includes["stb_image"]          = "%{VendorPaths.stb_image}/include"

Includes["imgui"]              = "%{MagmaVendorDir}"
Includes["yaml_cpp"]           = "%{VendorPaths.yaml_cpp}/include"
Includes["ImGuiFileDialog"]    = "%{MagmaVendorDir}"
Includes["flecs"]              = "%{MagmaVendorDir}"
Includes["rapidjson"]          = "%{VendorPaths.rapidjson}/include"
Includes["PhysX"]              = "%{VendorPaths.PhysX}/physx/include"
