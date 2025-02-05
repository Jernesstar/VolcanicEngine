rule "ASM"
    display "ASM Compile for angelscript"
    fileextension ".asm"
    location "%{RootPath}/build"

    buildmessage "Compiling %(Filename) ASM file for MSVC build"
    buildcommands 'ml64.exe /c /nologo /Fo"%(FullPath)" /W3 /Zi /Ta "%(IntDir)/%(Filename).obj"'
    buildoutputs '%(IntDir)/%(Filename).obj'

project "angelscript"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

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

    filter { "action:vs", "system:windows" }
        rules { "ASM" }
        files {
            "%{VendorPaths.angelscript}/angelscript/source/as_callfunc_x64_msvc_asm.asm"
        }

    -- filter 'files:**.asm'
    --     buildmessage 'Compiling %[%{file.relpath}]'

    --     -- One or more commands to run (required)
    --     buildcommands {
    --         'ml64.exe /c /nologo /Fo "%[%{!cfg.objdir}/%{file.basename}.obj]" /W3 /Zi /Ta "%[%{file.relpath}]"'
    --     }

    --     -- One or more outputs resulting from the build (required)
    --     buildoutputs { "%[%{!cfg.objdir}/%{file.basename}.obj]" }