VendorPath = "%{wks.location}/VolcaniCore/vendor"

VendorPaths = {}
VendorPaths["glm"] = "%{VendorPath}/glm"
VendorPaths["glfw"] = "%{VendorPath}/glfw"
VendorPaths["glad"] = "%{VendorPath}/glad"

Includes = {}
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["glad"] = "%{VendorPaths.glad}/include"

workspace "VolcaniCore"
    architecture "x64"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

include "VolcaniCore/config/glfw"
include "VolcaniCore/config/glad"
include "VolcaniCore"
include "Sandbox"