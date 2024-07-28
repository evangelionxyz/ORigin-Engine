-- Copyright (c) Evangelion Manuhutu | ORigin Engine

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
        optimize "off"
		symbols "on"
        defines {
            "DEBUG"
        }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"
        defines {
            "NDEBUG"
        }

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
        defines {
            "NDEBUG"
        }