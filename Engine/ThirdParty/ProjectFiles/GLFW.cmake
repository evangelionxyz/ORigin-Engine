add_library(GLFW STATIC
    ${THIRD_PARTY_DIR}/GLFW/src/context.c
    ${THIRD_PARTY_DIR}/GLFW/src/init.c
    ${THIRD_PARTY_DIR}/GLFW/src/input.c
    ${THIRD_PARTY_DIR}/GLFW/src/monitor.c
    ${THIRD_PARTY_DIR}/GLFW/src/null_init.c
    ${THIRD_PARTY_DIR}/GLFW/src/null_joystick.c
    ${THIRD_PARTY_DIR}/GLFW/src/null_monitor.c
    ${THIRD_PARTY_DIR}/GLFW/src/null_window.c
    ${THIRD_PARTY_DIR}/GLFW/src/platform.c
    ${THIRD_PARTY_DIR}/GLFW/src/vulkan.c
    ${THIRD_PARTY_DIR}/GLFW/src/window.c
    ${THIRD_PARTY_DIR}/GLFW/src/osmesa_context.c
    ${THIRD_PARTY_DIR}/GLFW/src/wgl_context.c
    ${THIRD_PARTY_DIR}/GLFW/src/egl_context.c
)

if (WIN32)
    target_compile_definitions(GLFW PRIVATE _GLFW_WIN32)
    target_sources(GLFW PRIVATE
        ${THIRD_PARTY_DIR}/GLFW/src/win32_init.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_joystick.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_module.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_monitor.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_time.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_thread.c
        ${THIRD_PARTY_DIR}/GLFW/src/win32_window.c
    )

elseif (UNIX AND NOT APPLE)
    
    target_compile_definitions(GLFW PRIVATE _GLFW_X11)
    message("[ORigin] GLFW: X11")

    target_sources(GLFW PRIVATE
        ${THIRD_PARTY_DIR}/GLFW/src/posix_time.c
        ${THIRD_PARTY_DIR}/GLFW/src/posix_thread.c
        ${THIRD_PARTY_DIR}/GLFW/src/posix_poll.c
        ${THIRD_PARTY_DIR}/GLFW/src/posix_module.c
        ${THIRD_PARTY_DIR}/GLFW/src/linux_joystick.c
        ${THIRD_PARTY_DIR}/GLFW/src/x11_init.c
        ${THIRD_PARTY_DIR}/GLFW/src/x11_monitor.c
        ${THIRD_PARTY_DIR}/GLFW/src/x11_window.c
        ${THIRD_PARTY_DIR}/GLFW/src/xkb_unicode.c
        ${THIRD_PARTY_DIR}/GLFW/src/glx_context.c
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(GLFW PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(GLFW PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(GLFW PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()