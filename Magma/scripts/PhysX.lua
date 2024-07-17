project "PhysX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        -- "%{VendorPaths.PhysX}/physx/**.h",
        "%{VendorPaths.PhysX}/physx/source/physx/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/physx/src/gpu/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/common/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/task/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/foundation/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/geomutils/src/**.cpp",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/api/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/software/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/common/src/pipeline/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/lowlevelaabb/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/lowleveldynamics/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtDefaultErrorCallback.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtDefaultCpuDispatcher.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtCpuWorkerThread.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtSimpleFactory.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtRigidBodyExt.cpp",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/ExtDefaultSimulationFilterShader.cpp",
        "%{VendorPaths.PhysX}/physx/source/scenequery/src/*.cpp",
        "%{VendorPaths.PhysX}/physx/source/simulationcontroller/src/*.cpp",
    }

    includedirs {
        "%{Includes.PhysX}",
        "%{Includes.PhysX}/foundation",
        "%{VendorPaths.PhysX}/physx/source/common/src",
        "%{VendorPaths.PhysX}/physx/source/common/include",
        "%{VendorPaths.PhysX}/physx/source/common/include/omnipvd",
        "%{VendorPaths.PhysX}/physx/source/pvd/include",
        "%{VendorPaths.PhysX}/physx/source/physxgpu/include",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/serialization/File",
        "%{VendorPaths.PhysX}/physx/source/physxextensions/src/serialization/Xml",
        "%{VendorPaths.PhysX}/physx/source/physxmetadata/core/include",
        "%{VendorPaths.PhysX}/physx/source/physxmetadata/extensions/include",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/common/include/**",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/api/include",
        "%{VendorPaths.PhysX}/physx/source/lowlevel/software/include",
        "%{VendorPaths.PhysX}/physx/source/lowlevelaabb/include",
        "%{VendorPaths.PhysX}/physx/source/lowleveldynamics/include",
        "%{VendorPaths.PhysX}/physx/source/lowleveldynamics/shared",
        "%{VendorPaths.PhysX}/physx/source/geomutils/include",
        "%{VendorPaths.PhysX}/physx/source/geomutils/src",
        "%{VendorPaths.PhysX}/physx/source/geomutils/src/**",
        "%{VendorPaths.PhysX}/physx/source/scenequery/include",
        "%{VendorPaths.PhysX}/physx/source/simulationcontroller/src",
        "%{VendorPaths.PhysX}/physx/source/simulationcontroller/include",
    }

    defines {
        -- "NDEBUG",
        "PX_PHYSX_STATIC_LIB",
        "PX_CHECKED",
        "PX_X64",
        "PX_X86"
    }

    buildoptions {
        "-O3",
        "-g3",
        "-gdwarf-2",
        "-std=c++0x",

        "-Wno-unused-macros",
        "-Wno-invalid-offsetof",
    }

    filter "system:linux"
        includedirs {
            "%{VendorPaths.PhysX}/physx/source/physx/src/device"
        }

        files {
            "%{VendorPaths.PhysX}/physx/source/foundation/unix/*.cpp",
            "%{VendorPaths.PhysX}/physx/source/physx/src/device/linux/*.cpp"
        }

        defines {
            "PX_LINUX",
            "PX_GCC"
        }

    filter "system:windows"
        includedirs {
            "%{VendorPaths.PhysX}/physx/source/physx/src/device"
        }

        files {
            "%{VendorPaths.PhysX}/physx/source/foundation/windows/*.cpp",
            "%{VendorPaths.PhysX}/physx/source/physx/src/device/windows/*.cpp"
        }