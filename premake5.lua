workspace "VolcanicEngine"
    location ("build/%{_ACTION}")
    architecture "x86_64"
    configurations { "Debug", "Release" }

    filter "system:linux"
        defines "VOLCANICENGINE_LINUX"

    filter "system:windows"
        defines {
            "VOLCANICENGINE_WINDOWS",
            "_DEBUG"
        }

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"

    filter "action:vs*"
        startproject "Editor"

include "VolcaniCore"
include "Magma"
include "Lava"
include "Ash"
include "Sandbox"
include "Test"

RootPath = _MAIN_SCRIPT_DIR;
VolcaniCoreVendorDir = "%{RootPath}/VolcaniCore/vendor"
MagmaVendorDir = "%{RootPath}/Magma/vendor"

VendorPaths = {}
Includes = {}

VendorPaths["glm"]  = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"] = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"] = "%{VolcaniCoreVendorDir}/glfw"

VendorPaths["imgui"]              = "%{MagmaVendorDir}/imgui"
VendorPaths["ImGuiFileDialog"]    = "%{MagmaVendorDir}/ImGuiFileDialog"
VendorPaths["ImGuizmo"]           = "%{MagmaVendorDir}/ImGuizmo"
VendorPaths["ImGuiColorTextEdit"] = "%{MagmaVendorDir}/ImGuiColorTextEdit"
VendorPaths["IconFontCppHeaders"] = "%{MagmaVendorDir}/IconFontCppHeaders"
VendorPaths["yaml_cpp"]           = "%{MagmaVendorDir}/yaml-cpp"
VendorPaths["flecs"]              = "%{MagmaVendorDir}/flecs"
VendorPaths["rapidjson"]          = "%{MagmaVendorDir}/rapidjson"
VendorPaths["PhysX"]              = "%{MagmaVendorDir}/PhysX"
VendorPaths["angelscript"]        = "%{MagmaVendorDir}/angelscript"
VendorPaths["assimp"]             = "%{MagmaVendorDir}/assimp"
VendorPaths["freetype"]           = "%{MagmaVendorDir}/freetype"
VendorPaths["stb_image"]          = "%{MagmaVendorDir}/stb_image"
VendorPaths["soloud"]             = "%{MagmaVendorDir}/soloud"
VendorPaths["efsw"]               = "%{MagmaVendorDir}/efsw"
VendorPaths["glslang"]            = "%{MagmaVendorDir}/glslang"
VendorPaths["SPIRV_Cross"]        = "%{MagmaVendorDir}/SPIRV-Cross"

Includes["glm"]  = "%{VendorPaths.glm}"
Includes["glad"] = "%{VendorPaths.glad}/include"
Includes["glfw"] = "%{VendorPaths.glfw}/include"

Includes["imgui"]              = "%{MagmaVendorDir}"
Includes["ImGuiFileDialog"]    = "%{MagmaVendorDir}"
Includes["ImGuizmo"]           = "%{MagmaVendorDir}"
Includes["ImGuiColorTextEdit"] = "%{MagmaVendorDir}"
Includes["IconFontCppHeaders"] = "%{MagmaVendorDir}"
Includes["yaml_cpp"]           = "%{VendorPaths.yaml_cpp}/include"
Includes["flecs"]              = "%{VendorPaths.flecs}/include"
Includes["rapidjson"]          = "%{VendorPaths.rapidjson}/include"
Includes["PhysX"]              = "%{VendorPaths.PhysX}/physx/include"
Includes["angelscript"]        = "%{VendorPaths.angelscript}/angelscript/include"
Includes["assimp"]             = "%{VendorPaths.assimp}/include"
Includes["freetype"]           = "%{VendorPaths.freetype}/include"
Includes["stb_image"]          = "%{VendorPaths.stb_image}/include"
Includes["soloud"]             = "%{VendorPaths.soloud}/include"
Includes["efsw"]               = "%{VendorPaths.efsw}/include"
Includes["glslang"]            = "%{VendorPaths.glslang}/glslang/Public"
Includes["SPIRV_Cross"]        = "%{VendorPaths.SPIRV_Cross}"
