add_library(MSDFGEN STATIC
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Contour.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/EdgeHolder.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/MSDFErrorCorrection.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Projection.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Scanline.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Shape.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/SignedDistance.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Vector2.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/contour-combiners.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-coloring.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-segments.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-selectors.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/equation-solver.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/msdf-error-correction.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/msdfgen.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/rasterization.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/render-sdf.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/save-bmp.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/save-tiff.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/sdf-error-estimation.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/shape-description.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/import-font.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/import-svg.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/resolve-shape-geometry.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/save-png.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/lib/lodepng.cpp
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/lib/tinyxml2.cpp
)

target_include_directories(MSDFGEN PRIVATE
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/include
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include
)

add_dependencies(MSDFGEN FREETYPE)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(MSDFGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(MSDFGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(MSDFGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()