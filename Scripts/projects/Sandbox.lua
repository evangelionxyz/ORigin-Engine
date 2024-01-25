-- Copyright (c) Evangelion Manuhutu | ORigin Engine

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
    "%{IncludeDir.FMOD}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.PhysX}",
    "%{IncludeDir.JoltPhysics}",
    "%{IncludeDir.YAML_CPP}",
}

defines {
    "NV_USE_STATIC_WINCRT",
    "GLFW_INCLUDE_NONE",
    "_CRT_SECURE_NO_WARNINGS",
    "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
}

linkoptions { "/ignore:4099,4006" }

filter "system:windows"
    systemversion "latest"
    postbuildcommands {
        -- Copy 3rd Party Library
        "{COPY} %{wks.location}ORigin/vendor/Assimp/lib/x64/assimp-vc143-mt.dll %{wks.location}Binaries/%{cfg.buildcfg}/Sandbox",
        "{COPY} %{wks.location}ORigin/vendor/FMOD/lib/fmod.dll %{wks.location}Binaries/%{cfg.buildcfg}/Sandbox"
    }

filter "configurations:Debug"
    defines {
        "PHYSX_CXX_FLAGS_DEBUG",
        "OGN_DEBUG",
        "_DEBUG"
    }
    runtime "Debug"
    symbols "On"

filter "configurations:Release"
    defines {
        "PX_PHYSX_STATIC_LIB",
        "OGN_RELEASE",
        "NDEBUG"
    }
    runtime "Release"
    optimize "On"

filter "configurations:Dist"
    defines {
        "PX_PHYSX_STATIC_LIB",
        "OGN_RELEASE",
        "NDEBUG"
    }
    runtime "Release"
    optimize "On"
