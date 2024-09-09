-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "FreeType"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
    language "C"
    staticruntime "off"
    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include/ft2build.h",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include/freetype/**.h",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include/freetype/config/**.h",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include/freetype/internal/**.h",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/autofit/autofit.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftbase.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftbbox.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftbdf.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftbitmap.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftcid.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftdebug.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftfstype.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftgasp.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftglyph.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftgxval.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftinit.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftmm.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftotval.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftpatent.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftpfr.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftstroke.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftsynth.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftsystem.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/fttype1.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/base/ftwinfnt.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/bdf/bdf.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/bzip2/ftbzip2.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/cache/ftcache.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/cff/cff.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/cid/type1cid.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/gzip/ftgzip.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/lzw/ftlzw.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/pcf/pcf.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/pfr/pfr.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/psaux/psaux.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/pshinter/pshinter.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/psnames/psnames.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/raster/raster.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/sdf/sdf.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/svg/svg.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/sfnt/sfnt.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/smooth/smooth.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/truetype/truetype.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/type1/type1.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/type42/type42.c",
        "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/src/winfonts/winfnt.c"
    }

    includedirs { "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include"}
    defines { "FT2_BUILD_LIBRARY" }
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
