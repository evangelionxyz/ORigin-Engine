-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "ORigin"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    location "%{wks.location}/Engine/ORigin"

    pchheader "pch.h"
    pchsource "sources/pch.cpp"

    targetdir (outputDir)
    objdir (intOutputDir)

    files {
        "sources/pch.cpp",
        "sources/pch.h",
        "sources/Origin/**.cpp",
        "sources/Origin/**.hpp",
        "sources/Origin/**.h",
        "sources/Platform/**.cpp",
        "sources/Platform/**.hpp",
        "sources/Platform/**.h",
    }

    includedirs {
        "sources",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.STBI}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.MINIAUDIO}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.FILEWATCHER}",
        "%{IncludeDir.YAMLCPP}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.MSDFGEN}",
        "%{IncludeDir.MSDFATLASGEN}",
        "%{IncludeDir.MONO}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.PhysX}",
    }

    links {
        "ASSIMP",
        "BOX2D",
        "GLFW",
        "GLAD",
        "IMGUI",
        "MSDFATLASGEN",
        "MSDFGEN",
        "FreeType",
        "YAMLCPP",
        "JOLT",
        "NvPhysX",
        "NvPhysXCooking",
        "NvPhysXCommon",
        "NvPhysXExtensions",
    }
    
    defines { "_CRT_SECURE_NO_WARNINGS" }
    buildoptions { "/utf-8" }

    -- ////////////////////////////////
    -- Windows
    filter "system:windows"
        systemversion "latest"
        links {
            "opengl32.lib",
            "%{Library.Vulkan1Lib}",
            "%{Library.FMOD}",
            "%{Library.MONO}",
            "%{Library.WinSock}",
            "%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}"
        }
        files {
            "sources/Platform/DX11/**.cpp",
            "sources/Platform/DX11/**.h",
            "sources/Platform/Win32/**.cpp",
            "sources/Platform/Win32/**.h",
        }

        postbuildcommands {
		      '{COPYFILE} "%{wks.location}/Engine/ThirdParty/FMOD/lib/win32/x64/fmod.dll" "%{cfg.targetdir}"',
	      }

        includedirs { "%{IncludeDir.VulkanSDK}" }

        defines {
            "GLFW_EXPOSE_NATIVE_WIN32",
            "OGN_PLATFORM_WINDOWS",
            "NOMINMAX",
            "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"
            defines { "OGN_DEBUG", "_DEBUG" }
            links {
                "%{Library.ShaderC_Debug}",
                "%{Library.SPIRV_Cross_Debug}",
                "%{Library.SPIRV_Cross_GLSL_Debug}",
                "%{Library.SPIRV_Tools_Debug}",
            }

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
            defines { "OGN_RELEASE", "NDEBUG" }
            links {
                "%{Library.ShaderC_Release}",
                "%{Library.SPIRV_Cross_Release}",
                "%{Library.SPIRV_Cross_GLSL_Release}",
            }

        filter "configurations:Dist"
            runtime "Release"
            symbols "off"
            optimize "on"
            defines { "OGN_RELEASE", "NDEBUG" }
            links {
                "%{Library.ShaderC_Release}",
                "%{Library.SPIRV_Cross_Release}",
                "%{Library.SPIRV_Cross_GLSL_Release}",
            }
    -- !Windows
    -- ////////////////////////////////


    -- ////////////////////////////////
    -- Linux
    filter "system:linux"
        defines { 
            "OGN_PLATFORM_LINUX", 
            "VK_VERSION_1_0", 
            "GLFW_INCLUDE_VULKAN",
            "_GLFW_X11"
        }
        pic "On"
        files { "%{prj.location}/sources/Platform/Linux/**.cpp" }
        libdirs { "/usr/lib" }
        includedirs { 
            "/usr/include/",
        }
        links {
            "vulkan", "shaderc_shared", "spirv-cross-core", "spirv-cross-glsl",
            "monosgen-2.0", "pthread", "dl", "m", "rt", "glib-2.0"
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"
            defines { "OGN_DEBUG", "_DEBUG" }

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
            defines { "OGN_RELEASE", "NDEBUG" }

        filter "configurations:Dist"
            runtime "Release"
            optimize "on"
            defines { "OGN_DISTRIBUTION", "NDEBUG" }
  
