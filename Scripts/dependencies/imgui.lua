-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "IMGUI"
  location (vendorProjectFiles)
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  targetdir (vendorOutputdir)
  objdir (vendorIntOutputdir)

  files {
    "%{wks.location}/ORigin/vendor/imgui/imgui_demo.cpp",
    "%{wks.location}/ORigin/vendor/imgui/imgui_draw.cpp",
    "%{wks.location}/ORigin/vendor/imgui/imgui_tables.cpp",
    "%{wks.location}/ORigin/vendor/imgui/imgui_widgets.cpp",
    "%{wks.location}/ORigin/vendor/imgui/imgui.cpp",
    "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_opengl3.cpp",
    "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_glfw.cpp",
    "%{wks.location}/ORigin/vendor/imgui/imconfig.h",
    "%{wks.location}/ORigin/vendor/imgui/imgui.h",
    "%{wks.location}/ORigin/vendor/imgui/imgui_internal.h",
    "%{wks.location}/ORigin/vendor/imgui/imstb_rectpack.h",
    "%{wks.location}/ORigin/vendor/imgui/imstb_textedit.h",
    "%{wks.location}/ORigin/vendor/imgui/imstb_truetype.h",

    -- include ImGuizmo src to compile
    "%{IncludeDir.IMGUIZMO}/ImGuizmo.cpp",
    "%{IncludeDir.IMGUIZMO}/ImGradient.cpp",
    "%{IncludeDir.IMGUIZMO}/GraphEditor.cpp",
    "%{IncludeDir.IMGUIZMO}/ImCurveEdit.cpp",
    "%{IncludeDir.IMGUIZMO}/ImSequencer.cpp",
  }

  includedirs {
    "%{wks.location}/ORigin/vendor/imgui",
    "%{wks.location}/ORigin/vendor/origin-glfw/include",
  }

  filter "system:linux"
    pic "On"

  filter "system:windows"
    systemversion "latest"
    files {
      "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_dx11.cpp",
      "%{wks.location}/ORigin/vendor/imgui/backends/imgui_impl_win32.cpp",
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"