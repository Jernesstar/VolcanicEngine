project "TestLib"
    kind "SharedLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("%{RootPath}/build/TestLib/obj")
    targetdir ("%{RootPath}/build/TestLib/lib")

    files {
        "func.cpp"
    }

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-fexceptions",
            "-Wno-format-security"
        }

    filter "system:windows"
        systemversion "latest"
