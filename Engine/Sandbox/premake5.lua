-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "Sandbox"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"
    location "%{wks.location}/Engine/Sandbox"

    links {"ORigin"}

    targetdir ("%{outputDir}/%{prj.name}")
    objdir (intOutputDir)

    files {
        "sources/**.cpp",
        "sources/**.hpp",
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
        "%{IncludeDir.MINIAUDIO}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.YAMLCPP}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.MSDFGEN}",
        "%{IncludeDir.MSDFATLASGEN}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.PhysX}",
        "%{IncludeDir.OPTICK}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    }

    buildoptions { "/utf-8" }

    filter "system:windows"
        includedirs { "%{IncludeDir.VulkanSDK}" }

        systemversion "latest"
        defines {
            "NV_USE_STATIC_WINCRT",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
            "GLFW_INCLUDE_VULKAN",
            "NOMINMAX"
        }

        filter "action:vs*"
          linkoptions { "/IGNORE:4099", "/IGNORE:4006", "/IGNORE:4217", "/IGNORE:4098" }

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