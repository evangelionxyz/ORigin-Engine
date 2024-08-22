-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "FreeType"
    location (vendorProjectFiles)
    kind "StaticLib"
    language "C"
    staticruntime "off"
    targetdir (vendorOutputdir)
    objdir (vendorIntOutputdir)

    files {
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include/ft2build.h",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include/freetype/**.h",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include/freetype/config/**.h",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include/freetype/internal/**.h",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/autofit/autofit.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbase.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbbox.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbdf.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbitmap.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftcid.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftdebug.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftfstype.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftgasp.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftglyph.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftgxval.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftinit.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftmm.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftotval.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftpatent.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftpfr.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftstroke.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftsynth.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftsystem.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/fttype1.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftwinfnt.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/bdf/bdf.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/bzip2/ftbzip2.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cache/ftcache.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cff/cff.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cid/type1cid.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/gzip/ftgzip.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/lzw/ftlzw.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pcf/pcf.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pfr/pfr.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/psaux/psaux.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pshinter/pshinter.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/psnames/psnames.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/raster/raster.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/sdf/sdf.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/svg/svg.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/sfnt/sfnt.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/smooth/smooth.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/truetype/truetype.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/type1/type1.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/type42/type42.c",
        "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/winfonts/winfnt.c"
    }

    includedirs { "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include"}
    defines { "FT2_BUILD_LIBRARY" }
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
