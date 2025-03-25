project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/lib")

    files {
        "src/VolcaniCore/**.h",
        "src/VolcaniCore/**.cpp",
        "src/impl/**.h",
        "src/impl/**.cpp"
    }

    includedirs {
        "src/",
        "src/VolcaniCore",
        "src/impl",

        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
    }

    links {
        "glfw",
        "glad",
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-Wno-format-security",
            "-Wno-pointer-arith"
        }

    filter "system:windows"
        systemversion "latest"


include "VolcaniCore/.builddeps/glfw"
include "VolcaniCore/.builddeps/glad"