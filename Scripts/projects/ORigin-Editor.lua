-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "Editor"
    location "%{wks.location}/ORigin-Editor"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    kind "ConsoleApp"

    targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
    objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

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
    
    links {
		"LowLevel",
		"LowLevelAABB",
		"LowLevelDynamics",
		"PhysXCommon",
		"PhysXFoundation",
		"PhysXExtensions",
		"PhysXPvdSDK",
		"PhysXTask",
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
