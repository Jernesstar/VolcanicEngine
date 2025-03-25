project "efsw"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.efsw}/src/efsw/*.cpp",
    }

    includedirs {
        "%{VendorPaths.efsw}/include",
        "%{VendorPaths.efsw}/src",
    }

    defines {
        "DEBUG"
    }

    filter "system:windows"
        files {
            "%{VendorPaths.efsw}/src/efsw/platform/win/*.cpp",
        }

    filter "system:linux"
        files {
            "%{VendorPaths.efsw}/src/efsw/platform/posix/*.cpp",
        }

    filter "system:mac"
        files {
            "%{VendorPaths.efsw}/src/efsw/platform/posix/*.cpp",
        }

