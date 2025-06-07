project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/VolcaniCore/lib")

    files {
        "%{VendorPaths.glad}/src/glad.c"
    }

    includedirs {
        "%{Includes.glad}"
    }