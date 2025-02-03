-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "Runtime"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"
    location "%{wks.location}/Engine/Runtime"

    targetdir (outputDir)
    objdir (intOutputDir)

    links {
        "ORigin",
        "NvPhysX",
        "NvPhysXCommon",
        "NvPhysXCooking",
        "NvPhysXExtensions",
        "OPTICK",
    }

    files {
        "sources/**.cpp",
        "sources/**.h",
        "sources/**.hpp",
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
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.PhysX}",
        "%{IncludeDir.OPTICK}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
    }

    filter "system:windows"
        includedirs { "%{IncludeDir.VulkanSDK}" }
        systemversion "latest"
        buildoptions { "/utf-8" }
        defines { "NV_USE_STATIC_WINCRT", "NOMINMAX", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
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
        debugdir (outputDir)