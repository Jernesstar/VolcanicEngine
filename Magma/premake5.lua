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
        "%{RootPath}/VolcaniCore/src/VolcaniCore",

        "%{Includes.glm}",
        "%{Includes.glfw}",
        "%{Includes.yaml_cpp}",
        "%{Includes.freetype}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
    }

    filter "system:windows"
        systemversion "latest"
