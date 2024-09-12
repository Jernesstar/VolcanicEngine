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
        "YAML_CPP_NO_CONTRIB"
    }

    buildoptions {
        "-w",
        "-Wall",
        "-Weffc++",
        "-Wno-long-long"
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-Wshadow",
            "-Wextra"
        }