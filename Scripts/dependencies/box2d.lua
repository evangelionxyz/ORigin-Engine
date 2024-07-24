-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "BOX2D"
  location (vendorProjectFiles)
  kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

   files {
		"%{wks.location}/ORigin/vendor/Box2D/src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/ORigin/vendor/Box2D/include",
		"%{wks.location}/ORigin/vendor/Box2D/src"
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

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"