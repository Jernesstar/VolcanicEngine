project "PhysX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        -- "%{VendorPaths.PhysX}/physx/**.h",
        "%{VendorPaths.PhysX}/physx/source/foundation/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/physx/src/NpPhysics.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtDefaultErrorCallback.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtSimpleFactory.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtRigidBodyExt.cpp",
        -- "%{VendorPaths.PhysX}/physx/source/physxextensions/src/*.cpp",
    }

    includedirs {
        "%{Includes.PhysX}",
        "%{Includes.PhysX}/foundation",
        "%{VendorPaths.PhysX}/physx/source/common/src",
        "%{VendorPaths.PhysX}/physx/source/common/include",
        -- "%{VendorPaths.PhysX}/physx/source/common/include/omnipvd",
        "%{VendorPaths.PhysX}/physx/source/pvd/include",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/**/**",
        -- "%{VendorPaths.PhysX}/physx/source/lowlevel/api/include",
        -- "%{VendorPaths.PhysX}/physx/source/lowlevel/software/include",
        -- "%{VendorPaths.PhysX}/physx/source/lowlevel/common/include/**",
        "%{VendorPaths.PhysX}/physx/source/lowlevelaabb/include",
        "%{VendorPaths.PhysX}/physx/source/lowleveldynamics/include",
        "%{VendorPaths.PhysX}/physx/source/lowleveldynamics/shared",
        "%{VendorPaths.PhysX}/physx/source/geomutils/include",
        "%{VendorPaths.PhysX}/physx/source/geomutils/src",
        "%{VendorPaths.PhysX}/physx/source/geomutils/src/**",
        -- "%{VendorPaths.PhysX}/physx/source/geomutils/src/hf",
        -- "%{VendorPaths.PhysX}/physx/source/geomutils/src/mesh",
        -- "%{VendorPaths.PhysX}/physx/source/geomutils/src/convex",
        "%{VendorPaths.PhysX}/physx/source/scenequery/include",
        "%{VendorPaths.PhysX}/physx/source/simulationcontroller/include",
        "%{VendorPaths.PhysX}/physx/source/simulationcontroller/src",
    }

    defines {
        "NDEBUG"
    }

    filter "system:linux"
        files {
            "%{VendorPaths.PhysX}/physx/source/foundation/unix/*.cpp"
        }

    filter "system:windows"
        files {
            "%{VendorPaths.PhysX}/physx/source/foundation/windows/*.cpp"
        }