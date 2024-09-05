add_executable(Sandbox
  sources/SandboxApp.cpp
  sources/SandboxLayer.cpp
  sources/SandboxLayer.h
)

add_dependencies(Sandbox ORigin)

target_include_directories(Sandbox PRIVATE
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
  ${VULKAN_INCLUDE_DIR}
)

add_dependencies(Sandbox ORigin)

target_link_libraries(Sandbox
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
    target_include_directories(Sandbox PRIVATE
        ${VENDOR_DIR}/mono/include
        ${VULKAN_INCLUDE_DIR}
    )

    target_compile_definitions(Sandbox PRIVATE
        _CRT_SECURE_NO_WARNINGS
        OGN_PLATFORM_WINDOWS
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )

    target_link_libraries(Sandbox
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
        target_link_libraries(Sandbox
            ${VULKAN_LIBRARY_DIR}/shaderc_sharedd.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-cored.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsld.lib
            ${VULKAN_LIBRARY_DIR}/SPIRV-Toolsd.lib
        )
    else()
        target_link_libraries(Sandbox
            ${VULKAN_LIBRARY_DIR}/shaderc_shared.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-core.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsl.lib
            ${VULKAN_LIBRARY_DIR}/SPIRV-Tools.lib
        )
    endif()
elseif (UNIX AND NOT APPLE)
        target_include_directories(Sandbox PRIVATE /usr/include)
        target_compile_definitions(Sandbox PRIVATE OGN_PLATFORM_LINUX)
        target_link_directories(Sandbox PRIVATE usr/lib)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(Sandbox PRIVATE
        OGN_DEBUG
        _DEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(Sandbox PRIVATE
        OGN_RELEASE
        _NDEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
     target_compile_definitions(Sandbox PRIVATE
        OGN_RELEASE
        OGN_DIST
        _NDEBUG
    )
endif()