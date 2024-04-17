VendorPath = "%{wks.location}/VolcaniCore/vendor"

VendorPaths = {}
VendorPaths["bx"] = "%{VendorPath}/bx"
VendorPaths["bgfx"] = "%{VendorPath}/bgfx"
VendorPaths["bimg"] = "%{VendorPath}/bimg"
VendorPaths["glm"] = "%{VendorPath}/glm"
VendorPaths["glad"] = "%{VendorPath}/glad"
VendorPaths["glfw"] = "%{VendorPath}/glfw"
VendorPaths["imgui"] = "%{VendorPath}/imgui"
VendorPaths["assimp"] = "%{VendorPath}/assimp"
VendorPaths["freetype"] = "%{VendorPath}/freetype"
VendorPaths["stb_image"] = "%{VendorPath}/stb_image"

Includes = {}
Includes["bx"] = "%{VendorPaths.bx}/include"
Includes["bgfx"] = "%{VendorPaths.bgfx}"
Includes["bimg"] = "%{VendorPaths.bimg}"
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glad"] = "%{VendorPaths.glad}/include"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["imgui"] = "%{VendorPath}"
Includes["assimp"] = "%{VendorPaths.assimp}/include"
Includes["freetype"] = "%{VendorPaths.freetype}/include"
Includes["stb_image"] = "%{VendorPaths.stb_image}/include"

workspace "VolcaniCore"
    architecture "x64"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

-- include "VolcaniCore/vendor/bx/scripts/bx"
include "VolcaniCore/vendor/bgfx/scripts/bgfx"
-- include "VolcaniCore/vendor/bimg/scripts/"
include "VolcaniCore/buildconfig/glad"
include "VolcaniCore/buildconfig/glfw"
include "VolcaniCore/buildconfig/imgui"
include "VolcaniCore/buildconfig/assimp"
include "VolcaniCore/buildconfig/freetype"
include "VolcaniCore/buildconfig/stb_image"
include "VolcaniCore"
include "Sandbox"