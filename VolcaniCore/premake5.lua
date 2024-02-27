project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin")
    objdir ("%{wks.location}/obj")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "src",

        "%{Includes.glfw}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
    }

    filter "system:windows"
        systemversion "latest"