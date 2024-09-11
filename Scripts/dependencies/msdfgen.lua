-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "MSDFGEN"
	location (ThirdPartyProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++11"
	staticruntime "off"

	links "freetype"

	targetdir (ThirdPartyOutputdir)
	objdir (ThirdPartyIntOutputdir)

	files {
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Contour.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/contour-combiners.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-coloring.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/EdgeHolder.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-segments.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/edge-selectors.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/equation-solver.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/msdf-error-correction.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/MSDFErrorCorrection.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/msdfgen.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Projection.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/rasterization.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/render-sdf.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/save-bmp.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/save-tiff.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Scanline.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/sdf-error-estimation.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Shape.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/shape-description.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/SignedDistance.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/core/Vector2.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/import-font.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/import-svg.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/resolve-shape-geometry.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/ext/save-png.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/lib/lodepng.cpp",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/lib/tinyxml2.cpp",
	}

	includedirs {
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/include",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/freetype/include"
	}

	defines { "MSDF_USE_CPP11" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
