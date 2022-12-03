-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

project "Box2D"
   location (vendorProjectFiles)
   kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "off"

   targetdir (vendorOutputdir)
   objdir (vendorIntOutputdir)

   files
	{
		"%{wks.location}/ORigin/vendor/Box2D/src/**.h",
		"%{wks.location}/ORigin/vendor/Box2D/src/**.cpp",
		"%{wks.location}/ORigin/vendor/Box2D/include/**.h"
	}

	includedirs
	{
		"%{wks.location}/ORigin/vendor/Box2D/include",
		"%{wks.location}/ORigin/vendor/Box2D/src"
	}

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