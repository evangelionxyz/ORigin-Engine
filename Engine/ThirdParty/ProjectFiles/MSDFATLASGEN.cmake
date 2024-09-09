add_library(MSDFATLASGEN STATIC
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/Charset.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/FontGeometry.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/GlyphGeometry.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/RectanglePacker.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/TightAtlasPacker.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/Workload.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/bitmap-blit.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/charset-parser.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/csv-export.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/glyph-generators.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/image-encode.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/json-export.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/main.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/shadron-preview-generator.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/size-selectors.cpp
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/utf8.cpp
)

 add_dependencies(MSDFATLASGEN MSDFGEN)

target_include_directories(MSDFATLASGEN PRIVATE
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen
    ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/include
)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(MSDFATLASGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(MSDFATLASGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(MSDFATLASGEN PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()