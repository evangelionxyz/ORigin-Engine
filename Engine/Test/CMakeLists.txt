add_executable("Test"
    sources/main.cpp
)

target_include_directories("Test" PRIVATE
  sources
  ${ROOT_DIR}/Engine/ORigin/sources
  ${THIRD_PARTY_DIR}/Assimp/include
  ${THIRD_PARTY_DIR}/Box2D/include
  ${THIRD_PARTY_DIR}/FMT/include
  ${THIRD_PARTY_DIR}/stb
  ${THIRD_PARTY_DIR}/GLFW/include
  ${THIRD_PARTY_DIR}/IMGUI
  ${THIRD_PARTY_DIR}/IMGUIZMO
  ${THIRD_PARTY_DIR}/glad/include
  ${THIRD_PARTY_DIR}/GLM
  ${THIRD_PARTY_DIR}/miniaudio
  ${THIRD_PARTY_DIR}/entt
  ${THIRD_PARTY_DIR}/Filewatcher/include
  ${THIRD_PARTY_DIR}/YAMLCPP/include
  ${THIRD_PARTY_DIR}/JOLTPHYSICS
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen
  ${THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen
)

add_dependencies("Test" ORigin)
target_link_libraries("Test" ORigin)

if (WIN32)
    target_include_directories("Test" PRIVATE
        ${THIRD_PARTY_DIR}/mono/include
        ${THIRD_PARTY_DIR}/Vulkan/Include
    )
    target_compile_definitions("Test" PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        OGN_PLATFORM_WINDOWS
        GLFW_INCLUDE_NONE
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )
elseif (UNIX AND NOT APPLE)
        target_include_directories("Test" PRIVATE
            /usr/include
            ${THIRD_PARTY_DIR}/mono/include
        )
        target_compile_definitions("Test" PRIVATE OGN_PLATFORM_LINUX)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties("Test" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
    )
    target_compile_definitions("Test" PRIVATE
        OGN_DEBUG
        _DEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties("Test" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
    )
    target_compile_definitions("Test" PRIVATE
        OGN_RELEASE
        _NDEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties("Test" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
    )
    target_compile_definitions("Test" PRIVATE
        OGN_RELEASE
        OGN_DIST
        _NDEBUG
    )
endif()