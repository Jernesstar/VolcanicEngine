project "shaderc"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.shaderc}"
    }

    includedirs {
        "%{Includes.shaderc}"
    }

    buildoptions  {
    }
