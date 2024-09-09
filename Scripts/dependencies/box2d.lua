-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "BOX2D"
  location (ThirdPartyProjectFiles)
  kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

   files {
        "%{THIRD_PARTY_DIR}/Box2D/src/**.cpp",
    }

    includedirs {
        "%{THIRD_PARTY_DIR}/Box2D/include",
        "%{THIRD_PARTY_DIR}/Box2D/src"
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