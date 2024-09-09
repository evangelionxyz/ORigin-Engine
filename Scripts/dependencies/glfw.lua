-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "GLFW"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/GLFW/src/context.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/init.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/input.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/monitor.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/null_init.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/null_joystick.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/null_monitor.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/null_window.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/platform.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/vulkan.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/window.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/osmesa_context.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/wgl_context.c",
        "%{THIRD_PARTY_DIR}/GLFW/src/egl_context.c",
    }

    -- Linux
    filter "system:linux"
        pic "On"
        files {
            "%{THIRD_PARTY_DIR}/GLFW/src/posix_time.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/posix_thread.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/posix_poll.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/posix_module.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/linux_joystick.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/x11_init.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/x11_monitor.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/x11_window.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/xkb_unicode.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/glx_context.c",
        }
        defines {
            "_GLFW_X11"
        }

    -- Windows
    filter "system:windows"
        systemversion "latest"
        files {
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_init.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_joystick.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_module.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_monitor.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_time.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_thread.c",
            "%{THIRD_PARTY_DIR}/GLFW/src/win32_window.c",
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