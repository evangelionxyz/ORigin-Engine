project "FMT"
    location (ThirdPartyProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/FMT/src/format.cc",
        "%{THIRD_PARTY_DIR}/FMT/src/os.cc",
        "%{THIRD_PARTY_DIR}/FMT/include/**.h",
    }

    includedirs {
        "%{THIRD_PARTY_DIR}/FMT/include"
    }

    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"
        defines { "WIN32", "_WINDOWS", "_UNICODE" }
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        symbols "on"
        optimize "full"

    filter "configurations:Dist"
        defines "NDEBUG"
        runtime "Release"
        optimize "full"
