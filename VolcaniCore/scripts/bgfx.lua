project "bgfx"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    exceptionhandling "Off"
    rtti "Off"
    defines "__STDC_FORMAT_MACROS"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

    files {
        path.join("%{VendorPaths.bgfx}", "include/bgfx/**.h"),
        path.join("%{VendorPaths.bgfx}", "src/*.cpp"),
        path.join("%{VendorPaths.bgfx}", "src/*.h"),
    }

    excludes { path.join("%{VendorPaths.bgfx}", "src/amalgamated.cpp") }

    includedirs {
        path.join("%{VendorPaths.bx}", "include"),
        path.join("%{VendorPaths.bimg}", "include"),
        path.join("%{VendorPaths.bgfx}", "include"),
        path.join("%{VendorPaths.bgfx}", "3rdparty"),
        path.join("%{VendorPaths.bgfx}", "3rdparty/dxsdk/include"),
        path.join("%{VendorPaths.bgfx}", "3rdparty/khronos"),
        path.join("%{VendorPaths.bgfx}", "3rdparty/directx-headers/include/**")
    }

    filter "action:vs*"
        defines "_CRT_SECURE_NO_WARNINGS"
        excludes {
            path.join("%{VendorPaths.bgfx}", "src/glcontext_glx.cpp"),
            path.join("%{VendorPaths.bgfx}", "src/glcontext_egl.cpp")
        }
    filter "system:macosx"
        files { path.join("%{VendorPaths.bgfx}", "src/*.mm") }

    filter "action:vs*"
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/msvc") }
    filter { "system:windows", "action:gmake2" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/mingw") }
    filter { "system:linux", "action:gmake2" }
        defines { "BX_PLATFORM_LINUX" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/linux") }
    filter { "system:macosx" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/osx") }
        buildoptions { "-x objective-c++" }


project "bimg"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    exceptionhandling "Off"
    rtti "Off"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

    files {
        path.join("%{VendorPaths.bimg}", "include/bimg/*.h"),
        path.join("%{VendorPaths.bimg}", "src/image.cpp"),
        path.join("%{VendorPaths.bimg}", "src/image_gnf.cpp"),
        path.join("%{VendorPaths.bimg}", "src/*.h"),
        path.join("%{VendorPaths.bimg}", "3rdparty/astc-encoder/source/*.cpp"),
    }

    includedirs {
        path.join("%{VendorPaths.bx}", "include"),
        path.join("%{VendorPaths.bimg}", "include"),
        path.join("%{VendorPaths.bimg}", "3rdparty/astc-encoder"),
        path.join("%{VendorPaths.bimg}", "3rdparty/astc-encoder/include/"),
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


project "bx"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    exceptionhandling "Off"
    rtti "Off"
    defines "__STDC_FORMAT_MACROS"

    objdir ("%{RootPath}/build/obj")
    targetdir ("%{RootPath}/build/libs")

    files {
        path.join("%{VendorPaths.bx}", "include/bx/*.h"),
        path.join("%{VendorPaths.bx}", "include/bx/inline/*.inl"),
        path.join("%{VendorPaths.bx}", "src/*.cpp")
    }

    excludes {
        path.join("%{VendorPaths.bx}", "src/amalgamated.cpp"),
        path.join("%{VendorPaths.bx}", "src/crtnone.cpp")
    }

    includedirs {
        path.join("%{VendorPaths.bx}", "3rdparty"),
        path.join("%{VendorPaths.bx}", "include")
    }

    filter "configurations:Release"
        defines "BX_CONFIG_DEBUG=0"
    filter "configurations:Debug"
        defines "BX_CONFIG_DEBUG=1"
    filter "action:vs*"
        defines "_CRT_SECURE_NO_WARNINGS"

    filter "action:vs*"
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/msvc") }
    filter { "system:windows", "action:gmake2" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/mingw") }
    filter { "system:linux", "action:gmake2" }
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/linux") }
    filter "system:macosx"
        includedirs { path.join("%{VendorPaths.bx}", "include/compat/osx") }
        buildoptions { "-x objective-c++" }
