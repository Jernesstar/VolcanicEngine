project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    exceptionhandling "Off"
    rtti "Off"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/bin")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{RootPath}/VolcaniCore/src",
        "%{RootPath}/VolcaniCore/impl",

        "%{Includes.bgfx}",
        "%{Includes.bimg}",
        "%{Includes.bx}",
        "%{Includes.glfw}",
        "%{Includes.glad}",
        "%{Includes.glm}",
        "%{Includes.imgui}",
        "%{Includes.assimp}",
        "%{Includes.freetype}",
        "%{Includes.stb_image}",
    }

    links {
        "VolcaniCore",
        -- "bgfx",
        "bimg",
        "bx",
        "glfw",
        "glad",
        "imgui",
        "assimp",
        "freetype",
        "stb_image",
    }

    filter "system:windows"
        systemversion "latest"
        links {
            "gdi32",
            -- "kernel32",
            -- "psapi"
        }
    filter "system:linux"
        links {
            "pthread",
            "dl",
            "GL",
            "X11"
        }
    filter "system:macosx"
        links {
            "QuartzCore.framework",
            "Metal.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

    filter "action:vs*"
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/msvc") }
    filter { "system:windows", "action:gmake2" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/mingw") }
    filter { "system:linux", "action:gmake2" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/linux") }
    filter "system:macosx"
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/osx") }
        buildoptions { "-x objective-c++" }
