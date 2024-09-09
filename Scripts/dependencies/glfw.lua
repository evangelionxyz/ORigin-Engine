-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "GLFW"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/origin-glfw/src/context.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/init.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/input.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/monitor.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/null_init.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/null_joystick.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/null_monitor.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/null_window.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/platform.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/vulkan.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/window.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/osmesa_context.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/wgl_context.c",
        "%{THIRD_PARTY_DIR}/origin-glfw/src/egl_context.c",
    }

    -- Linux
    filter "system:linux"
        pic "On"
        files {
            "%{THIRD_PARTY_DIR}/origin-glfw/src/posix_time.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/posix_thread.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/posix_poll.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/posix_module.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/linux_joystick.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/x11_init.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/x11_monitor.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/x11_window.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/xkb_unicode.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/glx_context.c",
        }
        defines {
            "_GLFW_X11"
        }

    -- Windows
    filter "system:windows"
        systemversion "latest"
        files {
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_init.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_joystick.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_module.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_monitor.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_time.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_thread.c",
            "%{THIRD_PARTY_DIR}/origin-glfw/src/win32_window.c",
        }

        defines {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"