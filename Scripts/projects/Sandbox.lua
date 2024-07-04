-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "Sandbox"
    location "%{wks.location}/Sandbox"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"

    links "ORigin"

    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/Sandbox")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/Sandbox")

    files {
        "%{prj.location}/sources/**.cpp",
    }

    includedirs {
        "%{wks.location}/ORigin/sources",
        "%{prj.location}/sources",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.STBI}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.Miniaudio}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.OPTICK}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.PhysX}",
        "%{IncludeDir.msdf_atlas_gen}",
        "%{IncludeDir.msdfgen}",
        "%{IncludeDir.JoltPhysics}",
        "%{IncludeDir.YAML_CPP}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    }

    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"
        defines { "NV_USE_STATIC_WINCRT", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    -- //////////////////////////////
    -- Default
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines {
            "PHYSX_CXX_FLAGS_DEBUG",
            "OGN_DEBUG", "_DEBUG"
        }

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines {
            "PX_PHYSX_STATIC_LIB",
            "OGN_RELEASE", "NDEBUG"
        }

    filter "configurations:Dist"
        optimize "On"
        runtime "Release"
        defines {
            "PX_PHYSX_STATIC_LIB",
            "OGN_RELEASE", "NDEBUG"
        }
