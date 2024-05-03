workspace "VolcanicEngine"
    location ("build")
    architecture "x64"
    configurations { "Debug", "Release" }

    if os.is64bit() and not os.istarget("windows") then
        platforms "x86_64"
    else
        platforms { "x86", "x86_64" }
    end

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

    filter "platforms:x86"
        architecture "x86"
    filter "platforms:x86_64"
        architecture "x86_64"

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
include "VolcaniCore"
include "Sandbox"

RootPath = _MAIN_SCRIPT_DIR;
VendorPath = "%{RootPath}/VolcaniCore/vendor"

VendorPaths = {}
VendorPaths["bgfx"] = "%{VendorPath}/bgfx"
VendorPaths["bimg"] = "%{VendorPath}/bimg"
VendorPaths["bx"] = "%{VendorPath}/bx"
VendorPaths["glm"] = "%{VendorPath}/glm"
VendorPaths["glad"] = "%{VendorPath}/glad"
VendorPaths["glfw"] = "%{VendorPath}/glfw"
VendorPaths["imgui"] = "%{VendorPath}/imgui"
VendorPaths["assimp"] = "%{VendorPath}/assimp"
VendorPaths["freetype"] = "%{VendorPath}/freetype"
VendorPaths["stb_image"] = "%{VendorPath}/stb_image"

Includes = {}
Includes["bgfx"] = "%{VendorPaths.bgfx}/include"
Includes["bimg"] = "%{VendorPaths.bimg}/include"
Includes["bx"] = "%{VendorPaths.bx}/include"
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glad"] = "%{VendorPaths.glad}/include"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["imgui"] = "%{VendorPath}"
Includes["assimp"] = "%{VendorPaths.assimp}/include"
Includes["freetype"] = "%{VendorPaths.freetype}/include"
Includes["stb_image"] = "%{VendorPaths.stb_image}/include"
