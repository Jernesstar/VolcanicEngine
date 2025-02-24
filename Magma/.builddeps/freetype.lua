project "freetype"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.freetype}/src/base/ftinit.c",
        "%{VendorPaths.freetype}/src/base/ftbase.c",
        "%{VendorPaths.freetype}/src/base/ftbbox.c",
        "%{VendorPaths.freetype}/src/base/ftdebug.c",
        "%{VendorPaths.freetype}/src/base/ftglyph.c",
        "%{VendorPaths.freetype}/src/base/ftsynth.c",
        "%{VendorPaths.freetype}/src/base/ftsystem.c",
        "%{VendorPaths.freetype}/src/base/ftbitmap.c",
        "%{VendorPaths.freetype}/src/base/ftstroke.c",
        "%{VendorPaths.freetype}/src/base/ftmm.c",
        "%{VendorPaths.freetype}/src/lzw/ftlzw.c",
        "%{VendorPaths.freetype}/src/sfnt/sfnt.c",
        "%{VendorPaths.freetype}/src/gzip/ftgzip.c",
        "%{VendorPaths.freetype}/src/truetype/truetype.c",
        "%{VendorPaths.freetype}/src/psnames/psnames.c",
        "%{VendorPaths.freetype}/src/raster/raster.c",
        "%{VendorPaths.freetype}/src/smooth/smooth.c",
        "%{VendorPaths.freetype}/src/autofit/autofit.c",
        "%{VendorPaths.freetype}/src/svg/svg.c"
    }

    includedirs {
        "%{Includes.freetype}"
    }

    buildoptions  {
        "-DFT2_BUILD_LIBRARY",
    }
