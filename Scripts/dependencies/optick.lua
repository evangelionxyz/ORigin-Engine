-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "OPTICK"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
	language "C++"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files { 
        "%{THIRD_PARTY_DIR}/OPTICK/src/**.cpp",
        "%{THIRD_PARTY_DIR}/OPTICK/src/**.h"
    }

    includedirs {
        "%{IncludeDir.VulkanSDK}"
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