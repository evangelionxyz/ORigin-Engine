-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "freetype"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C"
	staticruntime "off"
	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/autofit/autofit.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbase.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbbox.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbdf.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftbitmap.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftcid.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftdebug.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftfstype.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftgasp.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftglyph.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftgxval.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftinit.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftmm.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftotval.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftpatent.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftpfr.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftstroke.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftsynth.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftsystem.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/fttype1.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/base/ftwinfnt.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/bdf/bdf.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/bzip2/ftbzip2.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cache/ftcache.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cff/cff.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/cid/type1cid.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/gzip/ftgzip.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/lzw/ftlzw.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pcf/pcf.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pfr/pfr.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/psaux/psaux.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/pshinter/pshinter.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/psnames/psnames.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/raster/raster.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/sdf/sdf.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/svg/svg.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/sfnt/sfnt.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/smooth/smooth.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/truetype/truetype.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/type1/type1.c",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/type42/type42.c",
	}

	includedirs {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/include"
	}

	defines {
		"FT2_BUILD_LIBRARY",
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_WARNINGS"
	}

	filter "system:linux"
		pic "On"

	filter "system:windows"
		systemversion "latest"
		files {
			"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/freetype/src/winfonts/winfnt.c"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "msdfgen"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++11"
	staticruntime "off"
	links "freetype"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
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

	filter "system:linux"
		pic "On"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
