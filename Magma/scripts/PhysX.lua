project "PhysX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.PhysX}/*.h",
        "%{VendorPaths.PhysX}/*.cpp",
    }

    includedirs {
        "%{Includes.PhysX}",
    }
