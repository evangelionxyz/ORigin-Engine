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
  ${ROOT_DIR}/ORigin/sources
  ${VENDOR_DIR}/spdlog/include
  ${VENDOR_DIR}/Assimp/include
  ${VENDOR_DIR}/origin-glfw/include
  ${VENDOR_DIR}/glad/include
  ${VENDOR_DIR}/Box2D/include
  ${VENDOR_DIR}/entt
  ${VENDOR_DIR}/miniaudio
  ${VENDOR_DIR}/imgui
  ${VENDOR_DIR}/ImGuizmo
  ${VENDOR_DIR}/glm
  ${VENDOR_DIR}/yaml-cpp/include
  ${VENDOR_DIR}/JoltPhysics
  ${VENDOR_DIR}/msdf-atlas-gen/msdfgen
  ${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen
)

add_dependencies(Editor ORigin)

target_link_libraries(Editor
    ORigin
    ASSIMP
    BOX2D
    GLFW
    GLAD
    IMGUI
    MSDFATLASGEN
    MSDFGEN
    FreeType
    YAMLCPP
    JOLT
    fmt
)

if (WIN32)
    target_include_directories(Editor PRIVATE
        ${VENDOR_DIR}/mono/include
        ${VULKAN_INCLUDE_DIR}
    )

    target_compile_definitions(Editor PRIVATE
        _CRT_SECURE_NO_WARNINGS
        OGN_PLATFORM_WINDOWS
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )

    target_link_libraries(Editor
        opengl32.lib
        Ws2_32.lib
        Winmm.lib
        Version.lib
        Bcrypt.lib
        ${VENDOR_DIR}/mono/win32/libmono-static-sgen.lib
        ${VULKAN_LIBRARY_DIR}/vulkan-1.lib
    )

    # Vulkan
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        target_link_libraries(Editor
            ${VULKAN_LIBRARY_DIR}/shaderc_sharedd.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-cored.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsld.lib
        )
    else()
        target_link_libraries(Editor
            ${VULKAN_LIBRARY_DIR}/shaderc_shared.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-core.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsl.lib
        )
    endif()
elseif (UNIX AND NOT APPLE)
        target_include_directories(Editor PRIVATE
            ${VENDOR_DIR}/mono/include
            /usr/include
        )
        target_link_libraries(Editor
            vulkan
            shaderc_shared
            spirv-cross-core
            spirv-cross-glsl
        )
        target_compile_definitions(Editor PRIVATE OGN_PLATFORM_LINUX)
        target_link_directories(Editor PRIVATE usr/lib)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(Editor PRIVATE
        GLFW_INCLUDE_NONE
        OGN_DEBUG
        _DEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(Editor PRIVATE
        GLFW_INCLUDE_NONE
        OGN_RELEASE
        _NDEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
     target_compile_definitions(Editor PRIVATE
        GLFW_INCLUDE_NONE
        OGN_RELEASE
        OGN_DIST
        _NDEBUG
    )
endif()