project "soloud"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.soloud}/src/core/*.cpp",
        "%{VendorPaths.soloud}/src/backend/miniaudio/*.cpp",
        "%{VendorPaths.soloud}/src/audiosource/wav/*",
    }

    includedirs {
        "%{Includes.soloud}",
    }

    defines {
        "WITH_MINIAUDIO"
    }

    buildoptions {
        "-Wunused-result"
    }