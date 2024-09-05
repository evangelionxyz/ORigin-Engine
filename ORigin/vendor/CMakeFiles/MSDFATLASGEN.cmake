add_library(MSDFATLASGEN STATIC
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/Charset.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/FontGeometry.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/GlyphGeometry.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/RectanglePacker.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/TightAtlasPacker.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/Workload.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/bitmap-blit.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/charset-parser.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/csv-export.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/glyph-generators.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/image-encode.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/json-export.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/main.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/shadron-preview-generator.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/size-selectors.cpp
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen/utf8.cpp
)

 add_dependencies(MSDFATLASGEN MSDFGEN)

target_include_directories(MSDFATLASGEN PRIVATE
    ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen
    ${VENDOR_DIR}/msdf-atlas-gen/msdfgen/include
)