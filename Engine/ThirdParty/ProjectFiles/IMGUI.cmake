add_library(IMGUI
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.h
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.h
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h

    #ImGuizmo
    ${THIRD_PARTY_DIR}/ImGuizmo/GraphEditor.cpp
    ${THIRD_PARTY_DIR}/ImGuizmo/ImGuizmo.cpp
    ${THIRD_PARTY_DIR}/ImGuizmo/ImCurveEdit.cpp
    ${THIRD_PARTY_DIR}/ImGuizmo/ImGradient.cpp
    ${THIRD_PARTY_DIR}/ImGuizmo/ImSequencer.cpp
)

target_include_directories(IMGUI PRIVATE
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/GLFW/include
    ${THIRD_PARTY_DIR}/Vulkan/Include
)

if (WIN32)
    target_sources(IMGUI PRIVATE
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx11.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx11.h
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.h
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(IMGUI PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(IMGUI PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(IMGUI PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()