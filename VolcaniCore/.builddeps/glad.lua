project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/lib")

    files {
        "%{VendorPaths.glad}/include/*.h",
        "%{VendorPaths.glad}/src/glad.c"
    }

    includedirs {
        "%{Includes.glad}"
    }

    filter "system:windows"
        files {
            "%{VendorPaths.glad}/src/glad_wgl.c"
        }

    filter "system:linux"
        files {
            "%{VendorPaths.glad}/src/glad_glx.c"
        }
