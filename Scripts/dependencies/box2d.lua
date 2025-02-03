-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "BOX2D"
  location (ThirdPartyProjectFiles)
  kind "StaticLib"
    language "C"
    cdialect "C17"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

   files {
        "%{THIRD_PARTY_DIR}/BOX2D/src/**.c",
    }

    includedirs {
        "%{THIRD_PARTY_DIR}/BOX2D/include",
    }

    defines {
        "BOX2D_ENABLE_SIMD",
    }

    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"
        buildoptions {
            "/experimental:c11atomics"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"