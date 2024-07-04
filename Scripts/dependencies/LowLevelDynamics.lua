project "LowLevelDynamics"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location (vendorProjectFiles)
	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	includedirs {
		"%{physxDir}/physx/include",
		"%{physxDir}/physx/source/common/src/windows",
		"%{physxDir}/physx/source/lowlevel/software/include/windows",
		"%{physxDir}/physx/source/lowleveldynamics/include/windows",
		"%{physxDir}/physx/source/lowlevel/common/include/pipeline/windows",
		"%{physxDir}/physx/source/common/src",
		"%{physxDir}/physx/source/geomutils/src",
		"%{physxDir}/physx/source/geomutils/include",
		"%{physxDir}/physx/source/geomutils/src/contact",
		"%{physxDir}/physx/source/lowlevel/api/include",
		"%{physxDir}/physx/source/lowlevel/common/include",
		"%{physxDir}/physx/source/lowlevel/common/include/pipeline",
		"%{physxDir}/physx/source/lowlevel/common/include/utils",
		"%{physxDir}/physx/source/lowlevel/software/include",
		"%{physxDir}/physx/source/lowleveldynamics/include",
		"%{physxDir}/physx/source/lowleveldynamics/src",
		"%{physxDir}/physx/source/physxgpu/include",
	}
	files {  "%{physxDir}/physx/source/lowleveldynamics/**.cpp" }

	defines {
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1"
	}

	-- ///////////////////////////////
	-- Linux
	filter "system:linux"
		pic "On"

	-- ///////////////////////////////


	-- ///////////////////////////////
	-- Windows
	filter "system:windows"
		systemversion "latest"
		defines {
			"WIN32",
			"WIN64",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"
		defines {
			"PX_DEBUG=1",
			"PX_CHECKED=1",
			"PX_NVTX=0",
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines {
			"_HAS_ITERATOR_DEBUGGING=0",
			"_ITERATOR_DEBUG_LEVEL=0",
			"PX_CHECKED=1",
			"PX_NVTX=0",
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"
		defines {
			"_HAS_ITERATOR_DEBUGGING=0",
			"_ITERATOR_DEBUG_LEVEL=0",
			"PX_CHECKED=1",
		}