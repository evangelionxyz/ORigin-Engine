-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "Runtime"
    location "%{wks.location}/ORigin-Runtime"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"

    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

    links {
        "ORigin",
        "ASSIMP",
        "BOX2D",
        "GLFW",
        "GLAD",
        "IMGUI",
        "OPTICK",
        "MSDFATLASGEN",
        "MSDFGEN",
        "FreeType",
        "YAMLCPP",
        "JOLT",
    }

    files {
        "%{prj.location}/sources/**.cpp",
        "%{prj.location}/sources/**.h"
    }

    includedirs {
        "%{wks.location}/sources",
        "%{wks.location}/ORigin/sources",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.MINIAUDIO}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.YAMLCPP}",
        "%{IncludeDir.OPTICK}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.MSDFGEN}",
        "%{IncludeDir.MSDFATLASGEN}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
    }

    filter "system:linux"
        pic "On"
        includedirs { "%{IncludeDir.LinuxVulkanSDK}" }
        libdirs { "/usr/lib", "%{LibraryDir.LinuxVulkanSDK}", "%{LibraryDir.LinuxMono}" }
        links {
            "vulkan", "shaderc_shared", "spirv-cross-core", "spirv-cross-glsl",
            "monosgen-2.0", "pthread", "dl", "m", "rt"
        }

    filter "system:windows"
        systemversion "latest"
        defines { "NV_USE_STATIC_WINCRT", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

   -- //////////////////////////////
    -- Default
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DEBUG", "_DEBUG"
        }

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_RELEASE", "NDEBUG"
        }

    filter "configurations:Dist"
        optimize "On"
        runtime "Release"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DISTRIBUTION", "NDEBUG"
        }
