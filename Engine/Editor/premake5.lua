-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    location "%{wks.location}/Engine/Editor"

    targetdir (outputDir)
    objdir (intOutputDir)

    links {
        "ORigin",
        "NvPhysX",
        "NvPhysXCommon",
        "NvPhysXCooking",
        "NvPhysXExtensions",
    }

    files {
        "sources/**.cpp",
        "sources/**.h",
        "sources/**.hpp",
        "Resouces/Shaders/**.glsl",
        "**.h",
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
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    }

    buildoptions { "/utf-8" }
    
    -- ///////////////////////////////////////////
    -- Windows
    filter "system:windows"
        systemversion "latest"
        files {
          "%{prj.location}/Editor.rc",
        }
        defines {
            "NOMINMAX",
            "NV_USE_STATIC_WINCRT",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
        }

        filter "action:vs*"
          linkoptions { "/IGNORE:4099", "/IGNORE:4006", "/IGNORE:4217", "/IGNORE:4098" }

        includedirs { "%{IncludeDir.VulkanSDK}" }

    -- ///////////////////////////////////////////
    -- Linux
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
        defines {
            "VK_VERSION_1_0",
            "_GLFW_WAYLAND",
            "GLFW_INCLUDE_VULKAN"
        }

    -- //////////////////////////////
    -- Default
    filter "configurations:Debug"
        runtime "Debug"
        optimize "off"
        symbols "on"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DEBUG", "_DEBUG"
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        symbols "off"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_RELEASE", "NDEBUG"
        }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
        kind "WindowedApp"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DISTRIBUTION", "NDEBUG"
        }
        debugdir (outputDir)
