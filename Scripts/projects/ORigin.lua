-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Engine Project
project "ORigin"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  staticruntime "off"
  location "%{wks.location}/ORigin"

  targetdir ("%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin")
  objdir ("%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin")

  pchheader "pch.h"
  -- (pchsource) Under Windows, it must be a path relative to 'premake5.lua' (default premake file)
  pchsource "../../ORigin/sources/pch.cpp" -- don't change this

  files {
    "%{prj.location}/sources/Origin/**.cpp",
    "%{prj.location}/sources/Platform/OpenGL/**.cpp",

    "%{IncludeDir.STBI}/**.cpp",
    "%{IncludeDir.STBI}/**.h",
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.h",
    "%{IncludeDir.IMGUIZMO}/ImGradient.cpp",
    "%{IncludeDir.IMGUIZMO}/ImGradient.h",
    "%{IncludeDir.IMGUIZMO}/GraphEditor.cpp",
    "%{IncludeDir.IMGUIZMO}/GraphEditor.h",
    "%{IncludeDir.IMGUIZMO}/ImCurveEdit.cpp",
    "%{IncludeDir.IMGUIZMO}/ImCurveEdit.h",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.cpp",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.h"
  }

  includedirs {
    "%{prj.location}/sources",
    "%{IncludeDir.BOX2D}",
    "%{IncludeDir.SPDLOG}",
    "%{IncludeDir.STBI}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.IMGUI}",
    "%{IncludeDir.IMGUIZMO}",
    "%{IncludeDir.GLAD}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.Miniaudio}",
    "%{IncludeDir.ENTT}",
    "%{IncludeDir.MONO}",
    "%{IncludeDir.FILEWATCHER}",
    "%{IncludeDir.OPTICK}",
    "%{IncludeDir.YAML_CPP}",
    "%{IncludeDir.ASSIMP}",
    "%{IncludeDir.PhysX}",
    "%{IncludeDir.JoltPhysics}",
    "%{IncludeDir.msdfgen}",
    "%{IncludeDir.msdf_atlas_gen}"
  }

  defines {
    "GLFW_INCLUDE_NONE",
    "_CRT_SECURE_NO_WARNINGS"
  }

  links {
    "assimp",
    "Box2D",
    "origin-glfw",
    "glad",
    "ImGui",
    "Optick",
    "msdf-atlas-gen",
    "msdfgen",
    "yaml-cpp",
    "opengl32.lib",
    "PhysX",
    "PhysXCommon",
    "PhysXFoundation",
    "PhysXPvdSDK",
    "PhysXExtensions",
    "PhysXCooking",
    "PhysXCharacterKinematic",
    "%{Library.Vulkan}",
    "%{Library.MONO}"
  }

  filter "files:../../ORigin/vendor/ImGuizmo/**.cpp"
  flags { "NoPCH" }


  -- Windows
filter "system:windows"
  systemversion "latest"
  files {
    "%{prj.location}/sources/DX11/**.cpp",
    "%{prj.location}/sources/Win32/**.cpp",
  }

  includedirs {
    "%{IncludeDir.WindowsVulkanSDK}",
  }

  defines {
    "OGN_PLATFORM_WINDOWS",
    "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
  }

  links {
    "%{Library.WinSock}",
    "%{Library.WinMM}",
    "%{Library.WinVersion}",
    "%{Library.BCrypt}"
  }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines {
      "OGN_DEBUG",
      "_DEBUG"
    }
    links {
      "%{Library.WSPIRV_Cross_Debug}",
      "%{Library.WSPIRV_Cross_GLSL_Debug}",
      "%{Library.WSPIRV_Tools_Debug}",
    }

    filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines {
      "OGN_RELEASE",
      "NDEBUG"
    }
    links {
      "%{Library.WShaderC_Release}",
      "%{Library.WSPIRV_Cross_Release}",
      "%{Library.WSPIRV_Cross_GLSL_Release}",
    }

    filter "configurations:Dist"
    runtime "Release"
    optimize "On"
    defines {
        "OGN_RELEASE",
        "NDEBUG"
    }
    links {
        "%{Library.WShaderC_Release}",
        "%{Library.WSPIRV_Cross_Release}",
        "%{Library.WSPIRV_Cross_GLSL_Release}",
    }

-- Linux
filter "system:linux"
  includedirs {
    "%{IncludeDir.LinuxVulkanSDK}",
  }
  defines {
    "OGN_PLATFORM_LINUX",
  }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines {
      "OGN_DEBUG",
      "_DEBUG"
    }

    links {
      "%{Library.LSPIRV_Cross_Debug}",
      "%{Library.LSPIRV_Cross_GLSL_Debug}",
      "%{Library.LSPIRV_Tools_Debug}",
    }

    filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines {
      "OGN_RELEASE",
      "NDEBUG"
    }
    links {
      "%{Library.WShaderC_Release}",
      "%{Library.LSPIRV_Cross_Release}",
      "%{Library.LSPIRV_Cross_GLSL_Release}",
    }

    filter "configurations:Dist"
    runtime "Release"
    optimize "On"
    defines {
        "OGN_RELEASE",
        "NDEBUG"
    }
    links {
        "%{Library.LShaderC_Release}",
        "%{Library.LSPIRV_Cross_Release}",
        "%{Library.LSPIRV_Cross_GLSL_Release}",
    }

