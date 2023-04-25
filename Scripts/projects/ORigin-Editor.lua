-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Sandbox Project
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
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.IMGUI}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.YAML_CPP}",
    "%{IncludeDir.ASSIMP}",
}

defines {
    "GLFW_INCLUDE_NONE",
    "_CRT_SECURE_NO_WARNINGS"
}

filter "system:windows"
    systemversion "latest"
    postbuildcommands {
        -- Copy C# Project Assets
        "{COPYDIR} %{prj.location}SandboxProject/Binaries ../Binaries/%{cfg.buildcfg}/ORigin/SandboxProject/Binaries",
        "{COPYDIR} %{prj.location}SandboxProject/Assets ../Binaries/%{cfg.buildcfg}/ORigin/SandboxProject/Assets",

        "{COPYDIR} %{prj.location}mono ../Binaries/%{cfg.buildcfg}/ORigin/mono",
        "{COPYDIR} %{prj.location}Resources ../Binaries/%{cfg.buildcfg}/ORigin/Resources",

        "{COPY}    %{prj.location}imgui.ini ../Binaries/%{cfg.buildcfg}/ORigin"
    }

filter "configurations:Debug"
    defines {
        "OGN_DEBUG",
        "_DEBUG"
    }
    runtime "Debug"
    symbols "On"
    links
    {
        "%{Library.ShaderC_Debug}",
        "%{Library.SPIRV_Cross_Debug}",
        "%{Library.SPIRV_Cross_GLSL_Debug}",
        "%{Library.SPIRV_Tools_Debug}",
    }

filter "configurations:Release"
    defines "OGN_RELEASE"
    runtime "Release"
    optimize "On"

filter "configurations:Dist"
    defines "OGN_DIST"
    runtime "Release"
    optimize "On"