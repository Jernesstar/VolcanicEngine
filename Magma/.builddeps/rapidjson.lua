project "rapidjson"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.rapidjson}/include/**.h",
    }

    includedirs {
        "%{Includes.rapidjson}",
    }
