project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.yaml_cpp}/src/**.h",
        "%{VendorPaths.yaml_cpp}/src/**.cpp",
    }

    includedirs {
        "%{Includes.yaml_cpp}",
    }

    defines {
        "YAML_CPP_STATIC_DEFINE",
    }

    buildoptions {
        "-w",
        "-Wall",
        "-Wextra",
        "-Wshadow",
        "-Weffc++",
        "-Wno-long-long"
    }
