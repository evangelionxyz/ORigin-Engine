-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Sandbox Project
project "Sandbox"
location "%{wks.location}/Sandbox"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
staticruntime "off"
links "ORigin"

targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/Sandbox")
objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/Sandbox")

files {
    -- source file
    "%{prj.location}/sources/**.h",
    "%{prj.location}/sources/**.cpp",

    -- shader file
    "%{prj.location}/assets/shaders/**.glsl",
    "%{prj.location}/assets/shaders/**.hlsl",
    "%{prj.location}/assets/shaders/**.vert",
    "%{prj.location}/assets/shaders/**.frag",
    -- img file
    "%{prj.location}/assets/textures/**.png",
    "%{prj.location}/assets/textures/**.jpg",
}
includedirs {
    "%{wks.location}/ORigin/sources",
    "%{prj.location}/sources",
    "%{IncludeDir.SPDLOG}",
    "%{IncludeDir.ASSIMP}",
    "%{IncludeDir.STBI}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.IMGUI}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.YAML_CPP}",
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

filter "configurations:Release"
    defines "OGN_RELEASE"
    runtime "Release"
    optimize "On"

filter "configurations:Dist"
    defines "OGN_DIST"
    runtime "Release"
    optimize "On"