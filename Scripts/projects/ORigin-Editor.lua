-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "Editor"
    location "%{wks.location}/ORigin-Editor"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

    links {
        "ORigin",
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
    }

    files {
        "%{prj.location}/sources/**.cpp",
        "%{prj.location}/sources/**.h",
        "%{prj.location}/**.h",
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
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.MSDFGEN}",
        "%{IncludeDir.MSDFATLASGEN}",
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    }

    -- ///////////////////////////////////////////
    -- Windows
    filter "system:windows"
        systemversion "latest"
        files {
          "%{prj.location}/Editor.rc",
        }
        defines {
          "NV_USE_STATIC_WINCRT",
          "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
        }

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
        kind "ConsoleApp"
        runtime "Debug"
        optimize "off"
        symbols "on"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DEBUG", "_DEBUG"
        }

    filter "configurations:Release"
        kind "ConsoleApp"
        runtime "Release"
        optimize "on"
        symbols "off"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_RELEASE", "NDEBUG"
        }

    filter "configurations:Dist"
        kind "WindowedApp"
        runtime "Release"
        optimize "on"
        symbols "off"
        defines {
            "GLFW_INCLUDE_NONE",
            "OGN_DISTRIBUTION", "NDEBUG"
        }
