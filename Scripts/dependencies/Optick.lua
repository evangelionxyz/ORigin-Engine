-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
require "cmake"
project "OPTICK"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
		"%{wks.location}/ORigin/vendor/Optick/src/**.h",
		"%{wks.location}/ORigin/vendor/Optick/src/**.cpp",
	}

	includedirs{
		"%{wks.location}/ORigin/vendor/Optick/src",
	}

	filter "system:windows"
		systemversion "latest"
		includedirs {
			"%{IncludeDir.VulkanSDK}",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"