project "Test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    -- exceptionhandling "On"
    -- rtti "Off"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Test/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Test/bin")

    files {
        "main.cpp",
    }

    includedirs {
        "%{Includes.flecs}",
    }

    links {
        "flecs",
    }

    defines {
        "flecs_STATIC"
    }

    filter "system:linux"
        links {
            "pthread",
            "dl",
            "GL",
            "X11",
        }

    filter "system:windows"
        systemversion "latest"
        links {
            "Ws2_32",
            "wsock32"
        }

    filter "toolset:msc"
        buildoptions {
            "/NODEFAULTLIB:library"
        }