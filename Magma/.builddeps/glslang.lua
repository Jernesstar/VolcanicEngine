project "glslang"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        -- "%{VendorPaths.glslang}/glslang/**/*.cpp"
        "%{VendorPaths.glslang}/glslang/GenericCodeGen/*.cpp",
        "%{VendorPaths.glslang}/glslang/CInterface/*.cpp",
        "%{VendorPaths.glslang}/glslang/MachineIndependent/**.cpp",
        "%{VendorPaths.glslang}/glslang/ResourceLimits/*.cpp",
        "%{VendorPaths.glslang}/SPIRV/*.cpp",
    }

    includedirs {
        "%{Includes.glslang}",
        "%{VendorPaths.glslang}",
        "%{VendorPaths.glslang}/glslang",
    }

    buildoptions {

    }
