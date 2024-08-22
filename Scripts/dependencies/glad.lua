-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "GLAD"
    location (vendorProjectFiles)
    kind "StaticLib"
	language "C"
    staticruntime "off"

    targetdir (vendorOutputdir)
    objdir (vendorIntOutputdir)

    files { "%{wks.location}/ORigin/vendor/glad/src/glad.c"}
    includedirs { "%{IncludeDir.GLAD}" }
    flags { 'NoPCH' }

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