project "glad"
    kind "StaticLib"
    language "C"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

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