project "flecs"
    kind "StaticLib"
    language "C"

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
        "FLECS_CUSTOM_BUILD",
        "FLECS_CPP",
        "FLECS_ALERTS",
        "FLECS_LOG",
        "FLECS_METRICS",
        "FLECS_STATS",
        "FLECS_SYSTEM",
        "FLECS_PIPELINE"
    }

    buildoptions {
        "-std=gnu99"
    }