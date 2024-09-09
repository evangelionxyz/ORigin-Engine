add_library(FREETYPE STATIC
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/ftconfig.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/ftheader.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/ftmodule.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/ftoption.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/ftstdlib.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/integer-types.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/mac-support.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/config/public-macros.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/freetype.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftadvanc.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftbbox.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftbdf.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftbitmap.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftbzip2.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftcache.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftchapters.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftcid.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftcolor.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftdriver.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/fterrdef.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/fterrors.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftfntfmt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftgasp.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftglyph.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftgxval.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftgzip.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftimage.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftincrem.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftlcdfil.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftlist.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftlogging.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftlzw.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftmac.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftmm.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftmodapi.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftmoderr.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftotval.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftoutln.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftparams.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftpfr.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftrender.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftsizes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftsnames.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftstroke.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftsynth.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftsystem.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/fttrigon.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/fttypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ftwinfnt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/autohint.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/cffotypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/cfftypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/compiler-macros.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftcalc.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftdebug.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftdrv.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftgloadr.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/fthash.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftmemory.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftmmtypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftobjs.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftpsprop.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftrfork.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftserv.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftstream.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/fttrace.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/ftvalid.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/psaux.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/pshints.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svbdf.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svcfftl.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svcid.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svfntfmt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svgldict.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svgxval.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svkern.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svmetric.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svmm.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svotval.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svpfr.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svpostnm.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svprop.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svpscmap.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svpsinfo.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svsfnt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svttcmap.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svtteng.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svttglyf.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/services/svwinfnt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/sfnt.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/svginterface.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/t1types.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/tttypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/internal/wofftypes.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/otsvg.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/t1tables.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/ttnameid.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/tttables.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/freetype/tttags.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include/ft2build.h
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/autofit/autofit.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftbase.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftbbox.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftbdf.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftbitmap.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftcid.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftdebug.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftfstype.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftgasp.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftglyph.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftgxval.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftinit.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftmm.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftotval.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftpatent.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftpfr.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftstroke.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftsynth.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftsystem.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/fttype1.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/base/ftwinfnt.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/bdf/bdf.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/bzip2/ftbzip2.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/cache/ftcache.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/cff/cff.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/cid/type1cid.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/gzip/ftgzip.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/lzw/ftlzw.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/pcf/pcf.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/pfr/pfr.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/psaux/psaux.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/pshinter/pshinter.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/psnames/psnames.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/raster/raster.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/sdf/sdf.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/sfnt/sfnt.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/smooth/smooth.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/svg/svg.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/truetype/truetype.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/type1/type1.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/type42/type42.c
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/src/winfonts/winfnt.c
)

target_include_directories(FREETYPE PRIVATE ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include)
target_compile_definitions(FREETYPE PRIVATE FT2_BUILD_LIBRARY)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(FREETYPE PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(FREETYPE PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(FREETYPE PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()