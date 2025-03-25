project "flecs"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.flecs}/distr/flecs.c",
    }

    includedirs {
        "%{VendorPaths.flecs}/distr",
    }

    defines {
        -- "flecs_EXPORTS",
        "FLECS_DEBUG",
        -- "FLECS_CPP_NO_AUTO_REGISTRATION",

        -- "FLECS_CUSTOM_BUILD",
        -- "FLECS_CPP",
        -- "FLECS_ALERTS",
        -- "FLECS_SYSTEM",
        -- "FLECS_PIPELINE",
        -- "FLECS_TIMER",
        -- "FLECS_LOG",
        -- "FLECS_METRICS",
        -- "FLECS_STATS",
    }

    filter "toolset:gcc"
        cdialect "gnu99"
        links {
            "Ws2_32"
        }

