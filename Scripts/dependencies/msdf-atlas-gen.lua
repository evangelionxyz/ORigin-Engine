require "cmake"
project "MSDFATLASGEN"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++17"
	staticruntime "off"

	links "msdfgen"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files { "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/**.cpp" }

	includedirs {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"