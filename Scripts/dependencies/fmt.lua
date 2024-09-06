project "FMT"
    location (vendorProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir (vendorOutputdir)
    objdir (vendorIntOutputdir)

    files {
        "%{wks.location}/ORigin/vendor/FMT/src/fmt.cc",
        "%{wks.location}/ORigin/vendor/FMT/src/format",
        "%{wks.location}/ORigin/vendor/FMT/src/os.cc",
    }

    includedirs {
        "%{wks.location}/ORigin/vendor/FMT/include"
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
    