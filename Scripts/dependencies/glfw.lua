-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- GLFW Dependencies
project "glfw"
    location (vendorProjectFiles)
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files
	{
		"%{wks.location}/ORigin/vendor/glfw/include/GLFW/glfw3.h",
		"%{wks.location}/ORigin/vendor/glfw/include/GLFW/glfw3native.h",
		"%{wks.location}/ORigin/vendor/glfw/src/glfw_config.h",
		"%{wks.location}/ORigin/vendor/glfw/src/context.c",
		"%{wks.location}/ORigin/vendor/glfw/src/init.c",
		"%{wks.location}/ORigin/vendor/glfw/src/input.c",
		"%{wks.location}/ORigin/vendor/glfw/src/monitor.c",

		"%{wks.location}/ORigin/vendor/glfw/src/null_init.c",
		"%{wks.location}/ORigin/vendor/glfw/src/null_joystick.c",
		"%{wks.location}/ORigin/vendor/glfw/src/null_monitor.c",
		"%{wks.location}/ORigin/vendor/glfw/src/null_window.c",

		"%{wks.location}/ORigin/vendor/glfw/src/platform.c",
		"%{wks.location}/ORigin/vendor/glfw/src/vulkan.c",
		"%{wks.location}/ORigin/vendor/glfw/src/window.c",
	}

	filter "system:windows"
		systemversion "latest"
		files
		{
			"%{wks.location}/ORigin/vendor/glfw/src/win32_init.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_joystick.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_module.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_monitor.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_time.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_thread.c",
			"%{wks.location}/ORigin/vendor/glfw/src/win32_window.c",
			"%{wks.location}/ORigin/vendor/glfw/src/wgl_context.c",
			"%{wks.location}/ORigin/vendor/glfw/src/egl_context.c",
			"%{wks.location}/ORigin/vendor/glfw/src/osmesa_context.c"
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