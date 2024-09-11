-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "MSDFATLASGEN"
	location (ThirdPartyProjectFiles)
	kind "StaticLib"
	language "C++"
	cppdialect "c++17"
	staticruntime "off"

	links "msdfgen"

	targetdir (ThirdPartyOutputdir)
	objdir (ThirdPartyIntOutputdir)

	files { "%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen/**.cpp" }

	includedirs {
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen",
		"%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen/include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"