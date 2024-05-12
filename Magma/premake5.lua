project "Magma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "src/Magma/**.h",
        "src/Magma/**.cpp"
    }

    includedirs {
        "src",
        "%{RootPath}/VolcaniCore/src",

        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.freetype}",
    }

    filter "system:windows"
        systemversion "latest"
