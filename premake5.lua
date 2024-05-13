workspace "VolcanicEngine"
    location ("build")
    architecture "x64"
    configurations { "Debug", "Release" }

    -- if os.is64bit() and not os.istarget("windows") then
    --     platforms "x86_64"
    -- else
    --     platforms { "x86", "x86_64" }
    -- end

    -- filter "platforms:x86"
    --     architecture "x86"
    -- filter "platforms:x86_64"
    --     architecture "x86_64"

    filter "system:linux"
        defines "VOLCANICENGINE_LINUX"

    filter "system:windows"
        defines "VOLCANICENGINE_WINDOWS"

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
            ["ALWAYS_SEARCH_USER_PATHS"] = "YES", -- This is the minimum version of macos we'll be able to run on
        }
    filter "action:vs*"
        startproject "Sandbox"


-- include "VolcaniCore/scripts/bgfx"
include "VolcaniCore/scripts/glfw"
include "VolcaniCore/scripts/glad"
include "VolcaniCore/scripts/imgui"
include "VolcaniCore/scripts/assimp"
include "VolcaniCore/scripts/freetype"
include "VolcaniCore/scripts/stb_image"

include "Magma/scripts/yaml-cpp"

include "VolcaniCore"
include "Magma"
include "Sandbox"

RootPath = _MAIN_SCRIPT_DIR;
VolcaniCoreVendorDir = "%{RootPath}/VolcaniCore/vendor"
MagmaVendorDir = "%{RootPath}/Magma/vendor"

VendorPaths = {}
VendorPaths["bgfx"] = "%{VolcaniCoreVendorDir}/bgfx"
VendorPaths["bimg"] = "%{VolcaniCoreVendorDir}/bimg"
VendorPaths["bx"] = "%{VolcaniCoreVendorDir}/bx"
VendorPaths["glm"] = "%{VolcaniCoreVendorDir}/glm"
VendorPaths["glad"] = "%{VolcaniCoreVendorDir}/glad"
VendorPaths["glfw"] = "%{VolcaniCoreVendorDir}/glfw"
VendorPaths["imgui"] = "%{VolcaniCoreVendorDir}/imgui"
VendorPaths["assimp"] = "%{VolcaniCoreVendorDir}/assimp"
VendorPaths["freetype"] = "%{VolcaniCoreVendorDir}/freetype"
VendorPaths["stb_image"] = "%{VolcaniCoreVendorDir}/stb_image"

VendorPaths["yaml_cpp"] = "%{MagmaVendorDir}/yaml-cpp"

Includes = {}
Includes["bgfx"] = "%{VendorPaths.bgfx}/include"
Includes["bimg"] = "%{VendorPaths.bimg}/include"
Includes["bx"] = "%{VendorPaths.bx}/include"
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glad"] = "%{VendorPaths.glad}/include"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["imgui"] = "%{VolcaniCoreVendorDir}"
Includes["assimp"] = "%{VendorPaths.assimp}/include"
Includes["freetype"] = "%{VendorPaths.freetype}/include"
Includes["stb_image"] = "%{VendorPaths.stb_image}/include"

Includes["yaml_cpp"] = "%{VendorPaths.yaml_cpp}/include"
