project "PhysXPvdSDK"
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	location (vendorProjectFiles)
	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	includedirs {
		"%{physxDir}/physx/include",
		"%{physxDir}/physx/source/pvd/include",
		"%{physxDir}/physx/source/filebuf/include",
		"%{physxDir}/physx/source/foundation/include",
	}

	files { 
		"%{physxDir}/physx/source/pvd/src/PxProfileEventImpl.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdDataStream.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdDefaultFileTransport.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdDefaultSocketTransport.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdImpl.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdMemClient.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdObjectModelMetaData.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdObjectRegistrar.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdProfileZoneClient.cpp",
		"%{physxDir}/physx/source/pvd/src/PxPvdUserRenderer.cpp",
	} 
	
	defines {
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1"
	}
	
	filter "system:linux"
		pic "On"
		systemversion "latest"
		
	filter "system:windows"
		systemversion "latest"
		defines {
			"WIN32",
			"WIN64",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS",
			"PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll",
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