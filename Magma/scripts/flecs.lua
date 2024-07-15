project "flecs"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.flecs}/*.h",
        "%{VendorPaths.flecs}/*.cpp",
    }

    includedirs {
        "%{Includes.flecs}",
    }
