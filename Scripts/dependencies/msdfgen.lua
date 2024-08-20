-- Copyright (c) Evangelion Manuhutu | ORigin Engine
require "cmake"
project "MSDFGEN"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++11"
	staticruntime "off"

	links "freetype"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/Contour.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/contour-combiners.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/edge-coloring.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/EdgeHolder.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/edge-segments.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/edge-selectors.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/equation-solver.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/msdf-error-correction.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/MSDFErrorCorrection.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/msdfgen.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/Projection.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/rasterization.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/render-sdf.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/save-bmp.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/save-tiff.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/Scanline.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/sdf-error-estimation.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/Shape.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/shape-description.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/SignedDistance.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/core/Vector2.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/ext/import-font.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/ext/import-svg.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/ext/resolve-shape-geometry.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/ext/save-png.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/lib/lodepng.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/lib/tinyxml2.cpp",
	}

	includedirs {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/include",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include"
	}

	defines { "MSDF_USE_CPP11" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
