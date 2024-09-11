-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "GLAD"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
	language "C"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files { "%{THIRD_PARTY_DIR}/GLAD/src/glad.c"}
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