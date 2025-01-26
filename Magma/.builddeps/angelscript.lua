project "angelscript"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.angelscript}/angelscript/source/*.cpp",
    }

    includedirs {
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
    }

    defines {

    }

    filter "system:linux"
        buildoptions {
            "-fPIC",
            "-Wl,-Bsymbolic"
        }
