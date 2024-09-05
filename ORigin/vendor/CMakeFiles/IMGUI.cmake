add_library(IMGUI
    ${VENDOR_DIR}/imgui/imgui.cpp
    ${VENDOR_DIR}/imgui/imgui_demo.cpp
    ${VENDOR_DIR}/imgui/imgui_draw.cpp
    ${VENDOR_DIR}/imgui/imgui_tables.cpp
    ${VENDOR_DIR}/imgui/imgui_widgets.cpp
    ${VENDOR_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${VENDOR_DIR}/imgui/backends/imgui_impl_opengl3.h
    ${VENDOR_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${VENDOR_DIR}/imgui/backends/imgui_impl_vulkan.h
    ${VENDOR_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${VENDOR_DIR}/imgui/backends/imgui_impl_glfw.h

    #ImGuizmo
    ${VENDOR_DIR}/ImGuizmo/GraphEditor.cpp
    ${VENDOR_DIR}/ImGuizmo/ImGuizmo.cpp
    ${VENDOR_DIR}/ImGuizmo/ImCurveEdit.cpp
    ${VENDOR_DIR}/ImGuizmo/ImGradient.cpp
    ${VENDOR_DIR}/ImGuizmo/ImSequencer.cpp
)

target_include_directories(IMGUI PRIVATE
    ${VENDOR_DIR}/imgui
    ${VENDOR_DIR}/origin-glfw/include
    ${VULKAN_INCLUDE_DIR}
)

if (WIN32)
    target_sources(IMGUI PRIVATE
        ${VENDOR_DIR}/imgui/backends/imgui_impl_dx11.cpp
        ${VENDOR_DIR}/imgui/backends/imgui_impl_dx11.h
        ${VENDOR_DIR}/imgui/backends/imgui_impl_win32.cpp
        ${VENDOR_DIR}/imgui/backends/imgui_impl_win32.h
    )
endif()