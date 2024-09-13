project "flecs"
    kind "StaticLib"
    language "C"
--    staticruntime "On"
    staticruntime "Off"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.flecs}/*.h",
        "%{VendorPaths.flecs}/flecs.c",
    }

    includedirs {
        "%{Includes.flecs}",
    }

    defines {
        "flecs_STATIC",
        "FLECS_DEBUG",
        -- "FLECS_CUSTOM_BUILD",
        -- "FLECS_CPP",
        -- "FLECS_SYSTEM",
        -- "FLECS_PIPELINE"
        -- "FLECS_ALERTS",
        -- "FLECS_LOG",
        -- "FLECS_METRICS",
        -- "FLECS_STATS",
    }

    filter "toolset:gcc or toolset:clang"
        cdialect "gnu99"