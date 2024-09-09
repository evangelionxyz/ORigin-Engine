-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "JOLT"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files{
        "%{THIRD_PARTY_DIR}/JoltPhysics/Jolt/**.cpp",
    }

    includedirs{
        "%{THIRD_PARTY_DIR}/JoltPhysics/"
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