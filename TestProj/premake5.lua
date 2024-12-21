project "TestProj"
    kind "SharedLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("%{RootPath}/build/TestProj/obj")
    targetdir ("%{RootPath}/build/TestProj/lib")

    files {
        "func.cpp"
    }

    includedirs {
        "%{RootPath}/VolcaniCore/src",
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security"
        }

    filter "system:windows"
        systemversion "latest"
