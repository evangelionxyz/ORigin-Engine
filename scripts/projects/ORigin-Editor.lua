-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Sandbox Project
project "ORigin-Editor"
    location (mainProjectFiles)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    links "ORigin"

    targetdir (mainOutputdir)
    objdir (mainIntOutputdir)

    files {
        -- source file
		"%{prj.location}/sources/**.h",
		"%{prj.location}/sources/**.cpp",
		"%{prj.location}/**.h",
		"%{prj.location}/**.rc",
		"%{prj.location}/**.aps",

        -- shader file
		"%{prj.location}/assets/shaders/**.glsl",
		"%{prj.location}/assets/shaders/**.hlsl",

        -- img file
        "%{prj.location}/assets/textures/**.png",
        "%{prj.location}/assets/textures/**.jpg",
        "%{prj.location}/**.ico",
        "%{prj.location}/**.png",
	}

    includedirs {
        "%{wks.location}/ORigin/sources",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.BOX2D}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.YAML_CPP}",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"
        postbuildcommands {
            "{COPY} %{prj.location}/imgui.ini %{mainOutputdir}",

            "{COPYDIR} %{prj.location}/assets %{mainOutputdir}/assets",
            "{COPYDIR} %{prj.location}/mono %{mainOutputdir}/mono",
            "{COPYDIR} %{prj.location}/resources %{mainOutputdir}/resources",
        }

    filter "configurations:Debug"
		defines "OGN_DEBUG"
        runtime "Debug"
		symbols "On"
        links
        {
            "%{Library.ShaderC_Debug}",
            "%{Library.SPIRV_Cross_Debug}",
            "%{Library.SPIRV_Cross_GLSL_Debug}",
            "%{Library.SPIRV_Tools_Debug}",
        }

    filter "configurations:Release"
		defines "OGN_RELEASE"
        runtime "Release"
		optimize "On"

    filter "configurations:Dist"
		defines "OGN_DIST"
        runtime "Release"
		optimize "On"