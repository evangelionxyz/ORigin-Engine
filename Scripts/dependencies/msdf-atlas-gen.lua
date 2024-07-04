include "msdfgen.lua"

project "msdf-atlas-gen"
	location (vendorProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++17"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/bitmap-blit.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/Charset.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/charset-parser.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/csv-export.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/FontGeometry.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/glyph-generators.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/GlyphGeometry.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/image-encode.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/json-export.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/main.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/RectanglePacker.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/shadron-preview-generator.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/size-selectors.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/TightAtlasPacker.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/utf8.cpp",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen/Workload.cpp",
	}

	includedirs {
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen",
		"%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen/include"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	links "msdfgen"

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