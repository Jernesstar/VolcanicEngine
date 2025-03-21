project "flecs"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.flecs}/distr/flecs.c",
        -- "%{VendorPaths.flecs}/src/*.c",
        -- "%{VendorPaths.flecs}/src/datastructures/*.c",
        -- "%{VendorPaths.flecs}/src/query/*.c",
        -- "%{VendorPaths.flecs}/src/storage/*.c",
    }

    includedirs {
        "%{VendorPaths.flecs}/distr",
    }

    defines {
        "flecs_EXPORTS",
        "FLECS_DEBUG",
        -- "FLECS_CPP_NO_AUTO_REGISTRATION",
        -- "FLECS_CUSTOM_BUILD",
        -- "FLECS_CPP",
        -- "FLECS_SYSTEM",
        -- "FLECS_PIPELINE",
        -- "FLECS_ALERTS",
        -- "FLECS_LOG",
        -- "FLECS_METRICS",
        -- "FLECS_STATS",
    }

    filter "toolset:gcc or toolset:clang"
        cdialect "gnu99"
        links {
            "Ws2_32"
        }

