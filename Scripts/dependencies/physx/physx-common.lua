project "NvPhysXCommon"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location (ThirdPartyProjectFiles)
targetdir (ThirdPartyOutputdir)
objdir (ThirdPartyIntOutputdir)

	includedirs {
		"%{PhysXDir}/physx/include",
		"%{PhysXDir}/physx/source/common/src/windows",
		"%{PhysXDir}/physx/source/common/include",
		"%{PhysXDir}/physx/source/common/src",
		"%{PhysXDir}/physx/source/geomutils/include",
		"%{PhysXDir}/physx/source/geomutils/src",
		"%{PhysXDir}/physx/source/geomutils/src/contact",
		"%{PhysXDir}/physx/source/geomutils/src/common",
		"%{PhysXDir}/physx/source/geomutils/src/convex",
		"%{PhysXDir}/physx/source/geomutils/src/distance",
		"%{PhysXDir}/physx/source/geomutils/src/sweep",
		"%{PhysXDir}/physx/source/geomutils/src/gjk",
		"%{PhysXDir}/physx/source/geomutils/src/intersection",
		"%{PhysXDir}/physx/source/geomutils/src/mesh",
		"%{PhysXDir}/physx/source/geomutils/src/hf",
		"%{PhysXDir}/physx/source/geomutils/src/pcm",
		"%{PhysXDir}/physx/source/geomutils/src/ccd",
		"%{PhysXDir}/physx/source/physxgpu/include",
		"%{PhysXDir}/physx/source/foundation/include",
	}
	files { 
		"%{PhysXDir}/physx/source/common/src/CmCollection.cpp",
		"%{PhysXDir}/physx/source/common/src/CmPtrTable.cpp",
		"%{PhysXDir}/physx/source/common/src/CmSerialize.cpp",
		"%{PhysXDir}/physx/source/common/src/CmVisualization.cpp",
		"%{PhysXDir}/physx/source/common/src/CmRadixSort.cpp",
		"%{PhysXDir}/physx/source/geomutils/**.cpp",
	} 
	
	links {
		"NvPhysXFoundation",
	}
	
	defines {
		"WIN32",
		"WIN64",
		"_CRT_SECURE_NO_DEPRECATE",
		"_CRT_NONSTDC_NO_DEPRECATE",
		"_WINSOCK_DEPRECATED_NO_WARNINGS",
		"PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll",
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		
	filter "system:windows"
		systemversion "latest"
		files {
			"%{PhysXDir}/physx/source/common/src/windows/CmWindowsDelayLoadHook.cpp",
			"%{PhysXDir}/physx/source/common/src/windows/CmWindowsModuleUpdateLoader.cpp",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"
		defines {
			"PX_DEBUG=1",
			"PX_CHECKED=1",
			"PX_NVTX=0",
			"_DEBUG"
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
			"NDEBUG"
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"
		defines {
			"_HAS_ITERATOR_DEBUGGING=0",
			"_ITERATOR_DEBUG_LEVEL=0",
			"PX_CHECKED=1",
			"NDEBUG"
		}