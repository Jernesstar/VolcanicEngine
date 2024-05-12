project "VolcaniCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"

    objdir ("%{RootPath}/build/VolcaniCore/obj")
    targetdir ("%{RootPath}/build/VolcaniCore/lib")

    files {
        "src/VolcaniCore/**.h",
        "src/VolcaniCore/**.cpp",
        "src/impl/**.h",
        "src/impl/**.cpp"
    }

    includedirs {
        "src/VolcaniCore",
        "src/impl",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.imgui}/imgui",
        "%{Includes.freetype}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}"
    }

    filter "system:windows"
        systemversion "latest"
