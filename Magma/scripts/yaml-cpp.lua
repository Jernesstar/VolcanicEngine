project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.yaml-cpp}/*.h",
        "%{VendorPaths.yaml-cpp}/*.cpp",
    }

    includedirs {
        "%{Includes.yaml-cpp}",
    }
