-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "ORigin"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    location "%{wks.location}/Engine/ORigin"

    targetdir (outputDir)
    objdir (intOutputDir)

    files {
        "sources/pch.cpp",
        "sources/pch.h",
        "sources/Origin.hpp",
        "sources/Origin/**.cpp",
        "sources/Origin/**.hpp",
        "sources/Origin/**.h",
        "sources/Platform/OpenGL/**.cpp",
        "sources/Platform/Vulkan/**.cpp",
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
        "%{IncludeDir.OPTICK}",
        "%{IncludeDir.KTX}",
    }

    links {
        "ASSIMP",
        "OPTICK",
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
    
    -- ////////////////////////////////
    -- Windows
    filter "system:windows"
        systemversion "latest"
        buildoptions { "/utf-8" }
        links {
            "opengl32.lib",
            "%{Library.Vulkan1Lib}",
            "%{Library.KTXLib}",
            "%{Library.FMOD}",
            "%{Library.MONO}",
            "%{Library.WinSock}",
            "%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}"
        }
        files {
            "sources/Platform/Win32/Win32Utils.cpp",
        }
        
        filter "action:vs*"
            linkoptions { "/IGNORE:4099", "/IGNORE:4006", "/IGNORE:4217", "/IGNORE:4098" }
            pchheader "pch.h"
            pchsource "sources/pch.cpp"

        postbuildcommands {
            '{COPYFILE} "%{wks.location}/Engine/ThirdParty/FMOD/lib/win32/x64/fmod.dll" "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Engine/ThirdParty/KTX/lib/windows/ktx.dll" "%{cfg.targetdir}"',
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
        files { "%{prj.location}/sources/Platform/Linux/LinuxUtils.cpp" }
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
