-- Copyright (c) Evangelion Manuhutu | ORigin Engine

-- GLFW Dependencies
project "origin-glfw"
    location (vendorProjectFiles)
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files
	{
		"%{wks.location}/ORigin/vendor/origin-glfw/include/GLFW/glfw3.h",
		"%{wks.location}/ORigin/vendor/origin-glfw/include/GLFW/glfw3native.h",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/glfw_config.h",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/context.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/init.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/input.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/monitor.c",

		"%{wks.location}/ORigin/vendor/origin-glfw/src/null_init.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/null_joystick.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/null_monitor.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/null_window.c",

		"%{wks.location}/ORigin/vendor/origin-glfw/src/platform.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/vulkan.c",
		"%{wks.location}/ORigin/vendor/origin-glfw/src/window.c",
	}

	filter "system:linux"
		pic "On"
		files {
			"%{wks.location}/ORigin/vendor/origin-glfw/src/posix_time.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/posix_thread.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/posix_poll.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/posix_module.c",
		}

	filter "system:windows"
		systemversion "latest"
		files {
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_init.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_joystick.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_module.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_monitor.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_time.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_thread.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/win32_window.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/wgl_context.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/egl_context.c",
			"%{wks.location}/ORigin/vendor/origin-glfw/src/osmesa_context.c"
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