-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- ORigin Editor Project
project "ORigin-Editor"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
staticruntime "off"
location "%{wks.location}/ORigin-Editor"

links "ORigin"

targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

files {
    -- source file
    "%{prj.location}/sources/**.h",
    "%{prj.location}/sources/**.cpp",
    "%{prj.location}/**.h",
    "%{prj.location}/**.rc",
    "%{prj.location}/**.aps",

    -- shader file
    "%{prj.location}/Resources/Shaders/**.glsl",
    "%{prj.location}/Resources/Shaders/**.hlsl",

    -- img file
    "%{prj.location}/Resources/UITextures/**.png",
    "%{prj.location}/Resources/UITextures/**.jpg",
    "%{prj.location}/**.ico",
    "%{prj.location}/**.png",
}

includedirs {
    "%{wks.location}/ORigin/sources",
    "%{IncludeDir.SPDLOG}",
    "%{IncludeDir.ASSIMP}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.FMOD}",
    "%{IncludeDir.IMGUI}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.YAML_CPP}",
    "%{IncludeDir.ASSIMP}",
    "%{IncludeDir.msdfgen}",
    "%{IncludeDir.msdf_atlas_gen}"
}

defines {
    "GLFW_INCLUDE_NONE",
    "_CRT_SECURE_NO_WARNINGS"
}

filter "system:windows"
    systemversion "latest"

filter "configurations:Debug"
    defines {
        "OGN_DEBUG",
        "_DEBUG"
    }
    runtime "Debug"
    symbols "On"
    links  {
        "%{Library.ShaderC_Debug}",
        "%{Library.SPIRV_Cross_Debug}",
        "%{Library.SPIRV_Cross_GLSL_Debug}",
        "%{Library.SPIRV_Tools_Debug}",
    }
    
    postbuildcommands {
        --"{COPYDIR} %{prj.location}mono %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/mono",
        --"{COPYDIR} %{prj.location}Resources %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/Resources",
        --"{COPY} %{prj.location}imgui.ini %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",

        -- Copy 3rd Party Library
        --"{COPY} %{wks.location}ORigin/vendor/Assimp/lib/x64/assimp-vc143-mt.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",
        --"{COPY} %{wks.location}ORigin/vendor/FMOD/lib/fmod.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin"
    }

filter "configurations:Release"
    defines "OGN_RELEASE"
    runtime "Release"
    optimize "On"
     postbuildcommands {
        "{COPYDIR} %{prj.location}mono %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/mono",
        "{COPYDIR} %{prj.location}Resources %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/Resources",
        "{COPY} %{prj.location}imgui.ini %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",

        -- Copy 3rd Party Library
        "{COPY} %{wks.location}ORigin/vendor/Assimp/lib/x64/assimp-vc143-mt.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",
        "{COPY} %{wks.location}ORigin/vendor/FMOD/lib/fmod.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin"
    }

filter "configurations:Dist"
    defines "OGN_DIST"
    runtime "Release"
    optimize "On"
     postbuildcommands {
        "{COPYDIR} %{prj.location}mono %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/mono",
        "{COPYDIR} %{prj.location}Resources %{wks.location}Binaries/%{cfg.buildcfg}/ORigin/Resources",
        "{COPY} %{prj.location}imgui.ini %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",

        -- Copy 3rd Party Library
        "{COPY} %{wks.location}ORigin/vendor/Assimp/lib/x64/assimp-vc143-mt.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin",
        "{COPY} %{wks.location}ORigin/vendor/FMOD/lib/fmod.dll %{wks.location}Binaries/%{cfg.buildcfg}/ORigin"
    }