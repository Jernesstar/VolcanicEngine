project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin")
    objdir ("%{wks.location}/obj")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "src",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",
    }

    links {
        "bgfx",
        "glfw",
        "glad",
        "imgui",
        "assimp",
        "stb_image",
    }

    filter "system:windows"
        systemversion "latest"
