add_executable(Editor
    resource.h
    sources/EditorApp.cpp
    sources/EditorLayer.cpp
    sources/EditorLayer.h
    sources/Gizmos/Gizmos.cpp
    sources/Gizmos/Gizmos.h
    sources/Panels/AnimationTimeline.cpp
    sources/Panels/AnimationTimeline.h
    sources/Panels/ContentBrowserPanel.cpp
    sources/Panels/ContentBrowserPanel.h
    sources/Panels/Dockspace.cpp
    sources/Panels/Dockspace.h
    sources/Panels/MaterialEditor.cpp
    sources/Panels/MaterialEditor.h
    sources/Panels/SceneHierarchyPanel.cpp
    sources/Panels/SceneHierarchyPanel.h
    sources/Panels/SpriteSheetEditor.cpp
    sources/Panels/SpriteSheetEditor.h
    sources/Panels/ThumbnailCache.cpp
    sources/Panels/ThumbnailCache.h
    sources/Panels/UIEditor.cpp
    sources/Panels/UIEditor.h
    sources/Serializer/EditorSerializer.cpp
    sources/Serializer/EditorSerializer.h
)

target_include_directories(Editor PRIVATE
  sources
  ${ROOT_DIR}/Engine/ORigin/sources
  ${THIRD_PARTY_DIR}/spdlog/include
  ${THIRD_PARTY_DIR}/Assimp/include
  ${THIRD_PARTY_DIR}/origin-glfw/include
  ${THIRD_PARTY_DIR}/glad/include
  ${THIRD_PARTY_DIR}/Box2D/include
  ${THIRD_PARTY_DIR}/entt
  ${THIRD_PARTY_DIR}/miniaudio
  ${THIRD_PARTY_DIR}/imgui
  ${THIRD_PARTY_DIR}/ImGuizmo
  ${THIRD_PARTY_DIR}/glm
  ${THIRD_PARTY_DIR}/yaml-cpp/include
  ${THIRD_PARTY_DIR}/JoltPhysics
  ${THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen
  ${THIRD_PARTY_DIR}/msdf-atlas-gen/msdf-atlas-gen
)

add_dependencies(Editor ORigin)
target_link_libraries(Editor ORigin)

if (WIN32)
    target_include_directories(Editor PRIVATE
        ${THIRD_PARTY_DIR}/mono/include
        ${THIRD_PARTY_DIR}/Vulkan/Include
    )
    target_compile_definitions(Editor PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        OGN_PLATFORM_WINDOWS
        GLFW_INCLUDE_NONE
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )
elseif (UNIX AND NOT APPLE)
        target_include_directories(Editor PRIVATE
            /usr/include
            ${THIRD_PARTY_DIR}/mono/include
        )
        target_compile_definitions(Editor PRIVATE OGN_PLATFORM_LINUX)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(Editor PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
    )
    target_compile_definitions(Editor PRIVATE
        OGN_DEBUG
        _DEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(Editor PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
    )
    target_compile_definitions(Editor PRIVATE
        OGN_RELEASE
        _NDEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(Editor PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
    )
    target_compile_definitions(Editor PRIVATE
        OGN_RELEASE
        OGN_DIST
        _NDEBUG
    )
endif()