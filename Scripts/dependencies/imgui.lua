-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "IMGUI"
  location (ThirdPartyProjectFiles)
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  targetdir (ThirdPartyOutputdir)
  objdir (ThirdPartyIntOutputdir)

  files {
    "%{THIRD_PARTY_DIR}/IMGUI/imgui_demo.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui_draw.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui_tables.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui_widgets.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_opengl3.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_glfw.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_vulkan.cpp",
    "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_vulkan.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imconfig.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imgui_internal.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imstb_rectpack.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imstb_textedit.h",
    "%{THIRD_PARTY_DIR}/IMGUI/imstb_truetype.h",
    -- include ImGuizmo src to compile
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
    "%{IncludeDir.IMGUIZMO}/ImGradient.cpp",
    "%{IncludeDir.IMGUIZMO}/GraphEditor.cpp",
    "%{IncludeDir.IMGUIZMO}/ImCurveEdit.cpp",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.cpp",
  }

  includedirs {
    "%{THIRD_PARTY_DIR}/IMGUI",
    "%{THIRD_PARTY_DIR}/GLFW/include",
    "%{IncludeDir.VulkanSDK}"
  }

  filter "system:linux"
    pic "On"

  filter "system:windows"
    systemversion "latest"
    files {
      "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_dx11.cpp",
      "%{THIRD_PARTY_DIR}/IMGUI/backends/imgui_impl_win32.cpp",
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"