-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "Sandbox"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"
    location "%{wks.location}/Engine/Sandbox"

    links {"ORigin"}

    targetdir (outputDir)
    objdir (intOutputDir)

    files {
        "sources/**.cpp",
        "sources/**.h"
    }

    includedirs {
        "sources",
        "%{wks.location}/Engine/ORigin/sources",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.FMT}",
        "%{IncludeDir.MINIAUDIO}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.YAMLCPP}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.MSDFGEN}",
        "%{IncludeDir.MSDFATLASGEN}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    }

    buildoptions { "/utf-8" }

    filter "system:windows"
        includedirs { "%{IncludeDir.VulkanSDK}" }

    filter "system:linux"
        pic "On"
        includedirs { 
            "/usr/include/",
        }
        libdirs { "/usr/lib" }
        links {
            "vulkan", "shaderc_shared", "spirv-cross-core", "spirv-cross-glsl",
            "monosgen-2.0", "pthread", "dl", "m", "rt"
        }

    filter "system:windows"
        systemversion "latest"
        defines {
            "NV_USE_STATIC_WINCRT",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
            "GLFW_INCLUDE_VULKAN",
            "NOMINMAX"
        }

    -- //////////////////////////////
    -- Default
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines {
            "OGN_DEBUG", "_DEBUG"
        }

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines {
            "OGN_RELEASE", "NDEBUG"
        }

    filter "configurations:Dist"
        optimize "On"
        runtime "Release"
        defines {
            "OGN_DISTRIBUTION", "NDEBUG"
        }
        debugdir (outputDir)