-- Copyright (c) Evangelion Manuhutu | ORigin Engine

-- ORigin Runtime Project
project "Runtime"
  location "%{wks.location}/ORigin-Runtime"
  language "C++"
  cppdialect "C++20"
  staticruntime "off"

  links "ORigin"

  targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
  objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

  files {
    "%{prj.location}/sources/**.h",
    "%{prj.location}/sources/**.cpp"
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
        pic "on"

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
    links  {
      "%{Library.ShaderC_Debug}",
      "%{Library.SPIRV_Cross_Debug}",
      "%{Library.SPIRV_Cross_GLSL_Debug}",
      "%{Library.SPIRV_Tools_Debug}",
    }

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
    runtime "Release"
    optimize "On"
