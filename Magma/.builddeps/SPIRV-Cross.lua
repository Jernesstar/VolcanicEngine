project "SPIRV-Cross"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.SPIRV_Cross}/"
    }

    includedirs {
        "%{Includes.SPIRV_Cross}"
    }

    buildoptions  {
    }
