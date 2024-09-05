add_library(GLFW STATIC
    ${VENDOR_DIR}/origin-glfw/src/context.c
    ${VENDOR_DIR}/origin-glfw/src/init.c
    ${VENDOR_DIR}/origin-glfw/src/input.c
    ${VENDOR_DIR}/origin-glfw/src/monitor.c
    ${VENDOR_DIR}/origin-glfw/src/null_init.c
    ${VENDOR_DIR}/origin-glfw/src/null_joystick.c
    ${VENDOR_DIR}/origin-glfw/src/null_monitor.c
    ${VENDOR_DIR}/origin-glfw/src/null_window.c
    ${VENDOR_DIR}/origin-glfw/src/platform.c
    ${VENDOR_DIR}/origin-glfw/src/vulkan.c
    ${VENDOR_DIR}/origin-glfw/src/window.c
    ${VENDOR_DIR}/origin-glfw/src/osmesa_context.c
    ${VENDOR_DIR}/origin-glfw/src/wgl_context.c
    ${VENDOR_DIR}/origin-glfw/src/egl_context.c
)

if (WIN32)
    target_compile_definitions(GLFW PRIVATE _GLFW_WIN32)
    target_sources(GLFW PRIVATE
        ${VENDOR_DIR}/origin-glfw/src/win32_init.c
        ${VENDOR_DIR}/origin-glfw/src/win32_joystick.c
        ${VENDOR_DIR}/origin-glfw/src/win32_module.c
        ${VENDOR_DIR}/origin-glfw/src/win32_monitor.c
        ${VENDOR_DIR}/origin-glfw/src/win32_time.c
        ${VENDOR_DIR}/origin-glfw/src/win32_thread.c
        ${VENDOR_DIR}/origin-glfw/src/win32_window.c
    )

elseif (UNIX AND NOT APPLE)
    
    target_compile_definitions(GLFW PRIVATE _GLFW_X11)
    message("[ORigin] GLFW: X11")

    target_sources(GLFW PRIVATE
        ${VENDOR_DIR}/origin-glfw/src/posix_time.c
        ${VENDOR_DIR}/origin-glfw/src/posix_thread.c
        ${VENDOR_DIR}/origin-glfw/src/posix_poll.c
        ${VENDOR_DIR}/origin-glfw/src/posix_module.c
        ${VENDOR_DIR}/origin-glfw/src/linux_joystick.c
        ${VENDOR_DIR}/origin-glfw/src/x11_init.c
        ${VENDOR_DIR}/origin-glfw/src/x11_monitor.c
        ${VENDOR_DIR}/origin-glfw/src/x11_window.c
        ${VENDOR_DIR}/origin-glfw/src/xkb_unicode.c
        ${VENDOR_DIR}/origin-glfw/src/glx_context.c
    )
endif()