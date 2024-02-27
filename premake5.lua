VendorPath = "%{wks.location}/VolcaniCore/vendor"

VendorPaths = {}
VendorPaths["glm"] = "%{VendorPath}/glm"
VendorPaths["glfw"] = "%{VendorPath}/glfw"

Includes = {}
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glfw"] = "%{VendorPaths.glfw}/include"

workspace "Volcanic"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

include "VolcaniCore/config/glfw"
include "VolcaniCore"
include "Sandbox"