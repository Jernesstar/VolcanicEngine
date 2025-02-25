workspace "VolcanicEngine"
    location ("build")
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
        startproject "Sandbox"

include "VolcaniCore"
include "Magma"
include "Lava"
include "Ash"
include "Sandbox"

RootPath = _MAIN_SCRIPT_DIR;
VolcaniCoreVendorDir = "%{RootPath}/VolcaniCore/vendor"
MagmaVendorDir = "%{RootPath}/Magma/vendor"

VendorPaths = {}
Includes = {}

VendorPaths["glm"]             = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"]            = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"]            = "%{VolcaniCoreVendorDir}/glfw"

VendorPaths["imgui"]           = "%{MagmaVendorDir}/imgui"
VendorPaths["ImGuiFileDialog"] = "%{MagmaVendorDir}/ImGuiFileDialog"
VendorPaths["ImGuizmo"]        = "%{MagmaVendorDir}/ImGuizmo"
VendorPaths["yaml_cpp"]        = "%{MagmaVendorDir}/yaml-cpp"
VendorPaths["flecs"]           = "%{MagmaVendorDir}/flecs"
VendorPaths["rapidjson"]       = "%{MagmaVendorDir}/rapidjson"
VendorPaths["PhysX"]           = "%{MagmaVendorDir}/PhysX"
VendorPaths["angelscript"]     = "%{MagmaVendorDir}/angelscript"
VendorPaths["assimp"]          = "%{MagmaVendorDir}/assimp"
VendorPaths["freetype"]        = "%{MagmaVendorDir}/freetype"
VendorPaths["stb_image"]       = "%{MagmaVendorDir}/stb_image"
VendorPaths["soloud"]          = "%{MagmaVendorDir}/soloud"

Includes["glm"]             = "%{VendorPaths.glm}"
Includes["glad"]            = "%{VendorPaths.glad}/include"
Includes["glfw"]            = "%{VendorPaths.glfw}/include"

Includes["imgui"]           = "%{MagmaVendorDir}"
Includes["ImGuiFileDialog"] = "%{MagmaVendorDir}"
Includes["ImGuizmo"]        = "%{MagmaVendorDir}"
Includes["yaml_cpp"]        = "%{VendorPaths.yaml_cpp}/include"
Includes["flecs"]           = "%{MagmaVendorDir}"
Includes["rapidjson"]       = "%{VendorPaths.rapidjson}/include"
Includes["PhysX"]           = "%{VendorPaths.PhysX}/physx/include"
Includes["angelscript"]     = "%{VendorPaths.angelscript}/angelscript/include"
Includes["assimp"]          = "%{VendorPaths.assimp}/include"
Includes["freetype"]        = "%{VendorPaths.freetype}/include"
Includes["stb_image"]       = "%{VendorPaths.stb_image}/include"
Includes["soloud"]          = "%{VendorPaths.soloud}/include"
