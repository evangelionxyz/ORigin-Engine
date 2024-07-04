-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "Editor"
    location "%{wks.location}/ORigin-Editor"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    links {
        "ORigin",
        "assimp",
        "Box2D",
        "origin-glfw",
        "glad",
        "ImGui",
        "Optick",
        "msdf-atlas-gen",
        "msdfgen",
        "yaml-cpp"
    }

    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

    files {
        "%{prj.location}/sources/**.cpp",
        "%{prj.location}/sources/**.h",
        "%{prj.location}/**.h",
    }

    includedirs {
        "%{wks.location}/ORigin/sources",
        "%{wks.location}/sources",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.Miniaudio}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.YAML_CPP}",
        "%{IncludeDir.OPTICK}",
        "%{IncludeDir.PhysX}",
        "%{IncludeDir.JoltPhysics}",
        "%{IncludeDir.msdfgen}",
        "%{IncludeDir.msdf_atlas_gen}",
    }

    defines {
        "NV_USE_STATIC_WINCRT",
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
    }


    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines {
            "PHYSX_CXX_FLAGS_DEBUG",
            "OGN_DEBUG",
            "_DEBUG"
        }
        kind "ConsoleApp"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
    defines {
            "PX_PHYSX_STATIC_LIB",
            "OGN_RELEASE",
            "NDEBUG"
        }
        kind "ConsoleApp"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines {
            "PX_PHYSX_STATIC_LIB",
            "OGN_RELEASE",
            "NDEBUG"
        }
        kind "ConsoleApp"
        optimize "On"
