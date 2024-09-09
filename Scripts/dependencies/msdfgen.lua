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
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/Contour.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/contour-combiners.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/edge-coloring.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/EdgeHolder.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/edge-segments.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/edge-selectors.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/equation-solver.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/msdf-error-correction.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/MSDFErrorCorrection.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/msdfgen.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/Projection.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/rasterization.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/render-sdf.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/save-bmp.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/save-tiff.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/Scanline.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/sdf-error-estimation.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/Shape.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/shape-description.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/SignedDistance.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/core/Vector2.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/ext/import-font.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/ext/import-svg.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/ext/resolve-shape-geometry.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/ext/save-png.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/lib/lodepng.cpp",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/lib/tinyxml2.cpp",
	}

	includedirs {
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/include",
		"%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen/freetype/include"
	}

	defines { "MSDF_USE_CPP11" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
