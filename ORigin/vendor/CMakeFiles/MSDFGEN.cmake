add_library(MSDFGEN STATIC
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/Contour.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/EdgeHolder.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/MSDFErrorCorrection.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/Projection.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/Scanline.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/Shape.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/SignedDistance.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/Vector2.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/contour-combiners.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/edge-coloring.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/edge-segments.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/edge-selectors.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/equation-solver.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/msdf-error-correction.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/msdfgen.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/rasterization.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/render-sdf.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/save-bmp.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/save-tiff.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/sdf-error-estimation.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/core/shape-description.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/ext/import-font.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/ext/import-svg.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/ext/resolve-shape-geometry.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/ext/save-png.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/lib/lodepng.cpp
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/lib/tinyxml2.cpp
)

target_include_directories(MSDFGEN PRIVATE
  ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/include
  ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/freetype/include
)

add_dependencies(MSDFGEN FreeType)