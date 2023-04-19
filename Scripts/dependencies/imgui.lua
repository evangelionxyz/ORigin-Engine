-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
project "ImGui"
    location (vendorProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (vendorOutputdir)
    objdir (vendorIntOutputdir)

    includedirs{
        "%{wks.location}/ORigin/vendor/imgui/",
        "%{wks.location}/ORigin/vendor/glfw/include",
    }

    files{
        "%{wks.location}/ORigin/vendor/imgui/imconfig.h",
        "%{wks.location}/ORigin/vendor/imgui/imgui.h",
        "%{wks.location}/ORigin/vendor/imgui/imgui.cpp",
        "%{wks.location}/ORigin/vendor/imgui/imgui_tables.cpp",
        "%{wks.location}/ORigin/vendor/imgui/imgui_draw.cpp",
        "%{wks.location}/ORigin/vendor/imgui/imgui_internal.h",
        "%{wks.location}/ORigin/vendor/imgui/imgui_widgets.cpp",
        "%{wks.location}/ORigin/vendor/imgui/imstb_rectpack.h",
        "%{wks.location}/ORigin/vendor/imgui/imstb_textedit.h",
        "%{wks.location}/ORigin/vendor/imgui/imstb_truetype.h",
        "%{wks.location}/ORigin/vendor/imgui/imgui_demo.cpp",
        "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_glfw.cpp",
        "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_opengl3.cpp",
    }

    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"