VendorPath = "%{wks.location}/Volcanic/vendor"

VendorPaths = {}
VendorPaths["glm"] = "%{VendorPath}/glm"
VendorPaths["glfw"] = "%{VendorPath}/glfw"
VendorPaths["imgui"] = "%{VendorPath}/imgui"

Includes = {}
Includes["glm"] = "%{VendorPaths.glm}"
Includes["glfw"] = "%{VendorPaths.glfw}/include"
Includes["imgui"] = "%{VendorPath}"

workspace "Volcanic"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

include "Volcanic/config/glfw"
include "Volcanic/config/imgui"
include "Volcanic"
include "Sandbox"