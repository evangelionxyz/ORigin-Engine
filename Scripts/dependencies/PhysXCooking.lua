project "PhysXCooking"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location (vendorProjectFiles)
	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	includedirs {
		"%{physxDir}/physx/include",
		"%{physxDir}/physx/source/common/include",
		"%{physxDir}/physx/source/common/src",
		"%{physxDir}/physx/source/geomutils/include",
		"%{physxDir}/physx/source/geomutils/src",
		"%{physxDir}/physx/source/geomutils/src/contact",
		"%{physxDir}/physx/source/geomutils/src/common",
		"%{physxDir}/physx/source/geomutils/src/convex",
		"%{physxDir}/physx/source/geomutils/src/distance",
		"%{physxDir}/physx/source/geomutils/src/sweep",
		"%{physxDir}/physx/source/geomutils/src/gjk",
		"%{physxDir}/physx/source/geomutils/src/intersection",
		"%{physxDir}/physx/source/geomutils/src/mesh",
		"%{physxDir}/physx/source/geomutils/src/hf",
		"%{physxDir}/physx/source/geomutils/src/pcm",
		"%{physxDir}/physx/source/geomutils/src/ccd",
		"%{physxDir}/physx/source/physxcooking/src",
		"%{physxDir}/physx/source/physxcooking/src/mesh",
		"%{physxDir}/physx/source/physxcooking/src/convex",
		"%{physxDir}/physx/source/physxgpu/include",
		"%{physxDir}/physx/source/foundation/include",
	}
	
	files {
		"%{physxDir}/physx/source/physxcooking/src/Cooking.cpp"
	}
	
	defines {
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1",
		"PX_COOKING"
	}

	links {
		--"PhysXCommon",
		--"PhysXFoundation",
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		
	filter "system:windows"
		systemversion "latest"
		files { 
			"%{physxDir}/physx/source/physxcooking/src/windows/WindowsCookingDelayLoadHook.cpp"
		} 
		defines {
			"WIN32",
			"WIN64",
			"PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS",
		}

	-- Default
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