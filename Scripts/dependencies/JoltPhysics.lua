-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

project "JOLT"
    location (vendorProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (vendorOutputdir)
    objdir (vendorIntOutputdir)

    includedirs{
        "%{wks.location}/ORigin/vendor/JoltPhysics/"
    }

    files{
        "%{wks.location}/ORigin/vendor/JoltPhysics/Jolt/**.cpp",
        "%{wks.location}/ORigin/vendor/JoltPhysics/Jolt/**.h"
    }

    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"