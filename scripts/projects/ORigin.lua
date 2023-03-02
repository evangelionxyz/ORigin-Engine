-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Engine Project
project "ORigin"
    location (mainProjectFiles)
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir (mainOutputdir)
    objdir (mainIntOutputdir)

    pchheader "pch.h"
    -- (pchsource) Under Windows, it must be a path relative to 'premake5.lua' (default premake file)
    pchsource "../../ORigin/sources/pch.cpp" -- don't change this

    files {
	  "%{wks.location}/ORigin/sources/**.h",
	  "%{wks.location}/ORigin/sources/**.cpp",
	  "%{wks.location}/readme.md",
      "%{IncludeDir.STBI}/**.cpp",
      "%{IncludeDir.STBI}/**.h",
      "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
      "%{IncludeDir.IMGUIZMO}/ImGuizmo.h",
	}

    includedirs {
        "%{wks.location}/ORigin/sources",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.STBI}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.MONO}",
        "%{IncludeDir.YAML_CPP}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.ASSIMP}",
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
        "yaml-cpp",
        "opengl32.lib",
        "%{Library.Vulkan}",
        "%{Library.VulkanUtils}",

        "%{Library.MONO}"
        
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
		defines "OGN_DEBUG"
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
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}",
        }
        
        filter "configurations:Dist"
		defines "OGN_DIST"
        runtime "Release"
		optimize "On"
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}",
        }