rule "ASM"
    display "ASM Compile for angelscript"
    fileextension ".asm"
    location "%{RootPath}/build/%{_ACTION}"

    buildmessage "Compiling %(Filename) ASM file for MSVC build"
    buildcommands 'ml64.exe /c /nologo /Fo"%(Outputs)as_callfunc_x64_msvc_asm.obj" /W3 /Zi /Ta "%(RootDir)%(Directory)%(Filename)%(Extension)"'
    buildoutputs '%(Outputs)as_callfunc_x64_msvc_asm.obj'

project "angelscript"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    rules { "ASM" }

    files {
        "%{VendorPaths.angelscript}/angelscript/source/*.cpp",
        "%{VendorPaths.angelscript}/add_on/**.cpp",
    }

    includedirs {
        "%{Includes.angelscript}",
        "%{VendorPaths.angelscript}",
    }

    defines {

    }

    filter { "action:vs*", "system:windows" }
        files {
            "%{VendorPaths.angelscript}/angelscript/source/as_callfunc_x64_msvc_asm.asm"
        }

        defines {
            "/NODEFAULTLIB:library"
        }