-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Engine Project
project "ORigin"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location "%{wks.location}/ORigin"

targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

pchheader "pch.h"
-- (pchsource) Under Windows, it must be a path relative to 'premake5.lua' (default premake file)
pchsource "../../ORigin/sources/pch.cpp" -- don't change this

files {
    "%{prj.location}/sources/**.h",
    "%{prj.location}/sources/**.cpp",
    "%{wks.location}/readme.md",
    "%{IncludeDir.STBI}/**.cpp",
    "%{IncludeDir.STBI}/**.h",
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.cpp",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.h",
}

includedirs {
    "%{prj.location}/sources",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.SPDLOG}",
    "%{IncludeDir.STBI}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.IMGUI}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.FMOD}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.MONO}",
    "%{IncludeDir.FILEWATCHER}",
    "%{IncludeDir.YAML_CPP}",
    "%{IncludeDir.VulkanSDK}",
    "%{IncludeDir.ASSIMP}",
    "%{IncludeDir.PhysX}",
    "%{IncludeDir.JoltPhysics}",
    "%{IncludeDir.msdfgen}",
    "%{IncludeDir.msdf_atlas_gen}",
}

defines {
    "GLFW_INCLUDE_NONE",
    "_CRT_SECURE_NO_WARNINGS"
}

links {
    "Box2D",
    "glfw",
    "glad",
    "ImGui",
    "msdf-atlas-gen",
    "msdfgen",
    "yaml-cpp",
    "opengl32.lib",
    "PhysX",
    "PhysXCommon",
    "PhysXFoundation",
    "PhysXPvdSDK",
    "PhysXExtensions",
    "PhysXCooking",
    "PhysXCharacterKinematic",

    "%{Library.Vulkan}",
    "%{Library.FMOD}",
    "%{Library.Assimp}",
    "%{Library.MONO}",
}

filter "files:../../ORigin/vendor/ImGuizmo/**.cpp"
flags { "NoPCH" }


filter "system:windows"
systemversion "latest"
defines "OGN_WINDOWS_PLATFORM"
links
{
    "%{Library.WinSock}",
    "%{Library.WinMM}",
    "%{Library.WinVersion}",
    "%{Library.BCrypt}"
}

filter "configurations:Debug"
    defines {
        "PX_PHYSX_STATIC_LIB",
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
    defines {
        "PX_PHYSX_STATIC_LIB",
        "OGN_RELEASE",
        "NDEBUG"
    }
    runtime "Release"
    optimize "On"
    links
    {
        "%{Library.ShaderC_Release}",
        "%{Library.SPIRV_Cross_Release}",
        "%{Library.SPIRV_Cross_GLSL_Release}",
    }

filter "configurations:Dist"
    defines {
        "PX_PHYSX_STATIC_LIB",
        "OGN_RELEASE",
        "NDEBUG"
    }
    runtime "Release"
    optimize "On"
    links {
        "%{Library.ShaderC_Release}",
        "%{Library.SPIRV_Cross_Release}",
        "%{Library.SPIRV_Cross_GLSL_Release}",
    }