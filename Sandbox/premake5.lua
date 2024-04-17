project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin")
    objdir ("%{wks.location}/obj")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{wks.location}/VolcaniCore/src",

        "%{Includes.bgfx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.assimp}",
        "%{Includes.stb_image}",
    }

    links {
        "VolcaniCore",
        "bgfx",
        "glfw",
        "glad",
        "imgui",
        "assimp",
        "stb_image",
    }

    filter "system:windows"
        systemversion "latest"

        links {
            "gdi32"
        }

    filter "system:linux"
        links {
            "pthread",
            "dl"
        }
