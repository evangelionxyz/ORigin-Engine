-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

project "ORigin"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    location "%{wks.location}/ORigin"

    pchheader "pch.h"
    pchsource "%{prj.location}/sources/pch.cpp"

    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

    files {
        "%{prj.location}/sources/pch.cpp",
        "%{prj.location}/sources/pch.h",
        "%{prj.location}/sources/Origin/**.cpp",
        "%{prj.location}/sources/Origin/**.h",

        "%{prj.location}/sources/Platform/OpenGL/**.cpp",
        "%{prj.location}/sources/Platform/OpenGL/**.h",

        "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
        "%{IncludeDir.IMGUIZMO}/ImGuizmo.h",
        "%{IncludeDir.IMGUIZMO}/ImGradient.cpp",
        "%{IncludeDir.IMGUIZMO}/ImGradient.h",
        "%{IncludeDir.IMGUIZMO}/GraphEditor.cpp",
        "%{IncludeDir.IMGUIZMO}/GraphEditor.h",
        "%{IncludeDir.IMGUIZMO}/ImCurveEdit.cpp",
        "%{IncludeDir.IMGUIZMO}/ImCurveEdit.h",
        "%{IncludeDir.IMGUIZMO}/ImSequencer.cpp",
        "%{IncludeDir.IMGUIZMO}/ImSequencer.h"
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
        "%{IncludeDir.Miniaudio}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.FILEWATCHER}",
        "%{IncludeDir.OPTICK}",
        "%{IncludeDir.YAML_CPP}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.PhysX}",
        "%{IncludeDir.JoltPhysics}",
        "%{IncludeDir.msdfgen}",
        "%{IncludeDir.msdf_atlas_gen}",
        "%{IncludeDir.MONO}"
    }

    links {
        "assimp",
        "Box2D",
        "origin-glfw",
        "glad",
        "ImGui",
        "Optick",
        "msdf-atlas-gen",
        "msdfgen",
        "freetype",
        "yaml-cpp",
        "PhysX"
    }
    
    defines { "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS" }

    -- ////////////////////////////////
    -- Windows
    filter "system:windows"
        systemversion "latest"
        links {
            "opengl32.lib",
            "%{Library.Vulkan}",
            "%{Library.MONO}",
            "%{Library.WinSock}",
            "%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}"
        }
        files {
            "%{prj.location}/sources/Platform/DX11/**.cpp",
            "%{prj.location}/sources/Platform/DX11/**.h",
            "%{prj.location}/sources/Platform/Win32/**.cpp",
            "%{prj.location}/sources/Platform/Win32/**.h",
        }
        includedirs { 
          "%{IncludeDir.VulkanSDK}"
        }

        defines {
            "OGN_PLATFORM_WINDOWS",
            "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "On"
            defines { "OGN_DEBUG", "_DEBUG" }
            links {
                "%{Library.ShaderC_Debug}",
                "%{Library.SPIRV_Cross_Debug}",
                "%{Library.SPIRV_Cross_GLSL_Debug}",
                "%{Library.SPIRV_Tools_Debug}",
            }

        filter "configurations:Release"
            runtime "Release"
            optimize "On"
            defines { "OGN_RELEASE", "NDEBUG" }
            links {
                "%{Library.ShaderC_Release}",
                "%{Library.SPIRV_Cross_Release}",
                "%{Library.SPIRV_Cross_GLSL_Release}",
            }

        filter "configurations:Dist"
            runtime "Release"
            optimize "On"
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
        pic "On"
        files { "%{prj.location}/sources/Platform/Linux/**.cpp" }
        includedirs { "%{IncludeDir.LinuxVulkanSDK}" }
        defines { "OGN_PLATFORM_LINUX" }
        libdirs { "/usr/lib" }
        links {
            "vulkan", "shaderc_shared", "spirv-cross-core", "spirv-cross-glsl",
            "monosgen-2.0", "pthread", "dl", "m", "rt", "glib-2.0"
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "On"
            defines { "OGN_DEBUG", "_DEBUG" }

        filter "configurations:Release"
            runtime "Release"
            optimize "On"
            defines { "OGN_RELEASE", "NDEBUG" }

        filter "configurations:Dist"
            runtime "Release"
            optimize "On"
            defines { "OGN_RELEASE", "NDEBUG" }
  
