project "flecs"
    kind "StaticLib"
    language "C"
--    staticruntime "On"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.flecs}/flecs.c",
    }

    includedirs {
        "%{Includes.flecs}",
    }

    defines {
        "FLECS_DEBUG",
        "FLECS_CPP_NO_AUTO_REGISTRATION",
        -- "FLECS_CUSTOM_BUILD",
        -- "FLECS_CPP",
        -- "FLECS_SYSTEM",
        -- "FLECS_PIPELINE"
        -- -- "FLECS_ALERTS",
        -- -- "FLECS_LOG",
        -- -- "FLECS_METRICS",
        -- -- "FLECS_STATS",
    }

    filter "toolset:gcc or toolset:clang"
        cdialect "gnu99"
