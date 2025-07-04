project "stb_image"
    kind "StaticLib"
    language "C"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.stb_image}/include/stb_image/stb_image.h",
        "%{VendorPaths.stb_image}/src/stb_image.cpp",
    }

    includedirs {
        "%{Includes.stb_image}"
    }
