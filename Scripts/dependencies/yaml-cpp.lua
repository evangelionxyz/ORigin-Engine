-- Copyright (c) Evangelion Manuhutu | ORigin Engine
require "cmake"
project "YAMLCPP"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
        "%{wks.location}/ORigin/vendor/yaml-cpp/src/**.cpp",
        "%{wks.location}/ORigin/vendor/yaml-cpp/src/**.h",
        "%{wks.location}/ORigin/vendor/yaml-cpp/include/yaml-cpp/**.h",
    }

	defines {
        "YAMLCPP_USE_STATIC_LIBS"
    }

    includedirs {
        "%{wks.location}/ORigin/vendor/yaml-cpp/include/"
    }

	filter "system:linux"
		pic "On"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"