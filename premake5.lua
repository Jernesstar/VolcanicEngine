workspace "VolcanicEngine"
    location ("build")
    architecture "x86_64"
    configurations { "Debug", "Release" }

    if os.is64bit() and not os.istarget("windows") then
        platforms "x86_64"
    else
        platforms { "x86", "x86_64" }
    end

    filter "platforms:x86"
        architecture "x86"
    filter "platforms:x86_64"
        architecture "x86_64"

    filter "system:linux"
        defines "VOLCANICENGINE_LINUX"

    filter "system:windows"
        defines {
            "VOLCANICENGINE_WINDOWS",
            "YAML_CPP_STATIC_DEFINE",
            "YAML_CPP_NO_CONTRIB"
        }
    filter "configurations:Debug*"
        defines {
            "_DEBUG",
            "BX_CONFIG_DEBUG=1"
        }
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "BX_CONFIG_DEBUG=0"
        }
        optimize "Full"

    filter "system:macosx"
        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "10.9",
            ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
        }
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

VendorPaths["bgfx"] = "%{VolcaniCoreVendorDir}/bgfx"
VendorPaths["bimg"] = "%{VolcaniCoreVendorDir}/bimg"
VendorPaths["bx"] = "%{VolcaniCoreVendorDir}/bx"
VendorPaths["glm"] = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"] = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"] = "%{VolcaniCoreVendorDir}/glfw"
VendorPaths["assimp"] = "%{VolcaniCoreVendorDir}/assimp"
VendorPaths["freetype"] = "%{VolcaniCoreVendorDir}/freetype"
VendorPaths["stb_image"] = "%{VolcaniCoreVendorDir}/stb_image"

VendorPaths["imgui"] = "%{MagmaVendorDir}/imgui"
VendorPaths["yaml_cpp"] = "%{MagmaVendorDir}/yaml-cpp"
VendorPaths["ImGuiFileDialog"] = "%{MagmaVendorDir}/ImGuiFileDialog"

Includes["bgfx"] = "%{VendorPaths.bgfx}/include"
Includes["bimg"] = "%{VendorPaths.bimg}/include"
Includes["bx"] = "%{VendorPaths.bx}/include"
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glad"] = "%{VendorPaths.glad}/include"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["assimp"] = "%{VendorPaths.assimp}/include"
Includes["freetype"] = "%{VendorPaths.freetype}/include"
Includes["stb_image"] = "%{VendorPaths.stb_image}/include"

Includes["imgui"] = "%{MagmaVendorDir}"
Includes["yaml_cpp"] = "%{VendorPaths.yaml_cpp}/include"
Includes["ImGuiFileDialog"] = "%{MagmaVendorDir}"
