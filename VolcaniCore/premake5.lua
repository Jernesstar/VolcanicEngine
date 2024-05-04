project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

    files {
        "src/**.h",
        "src/**.cpp",
        "impl/**.h",
        "impl/**.cpp",
    }

    includedirs {
        "src",
        "impl",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",
    }

    filter "system:windows"
        systemversion "latest"
