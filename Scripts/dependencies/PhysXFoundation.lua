project "PhysXFoundation"
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	location (vendorProjectFiles)
	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

	files { 
		"%{physxDir}/physx/source/foundation/FdAllocator.cpp",
		"%{physxDir}/physx/source/foundation/FdAssert.cpp",
		"%{physxDir}/physx/source/foundation/FdFoundation.cpp",
		"%{physxDir}/physx/source/foundation/FdMathUtils.cpp",
		"%{physxDir}/physx/source/foundation/FdString.cpp",
		"%{physxDir}/physx/source/foundation/FdTempAllocator.cpp",
	} 
	includedirs { "%{physxDir}/physx/include" }

	defines {
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		files {
			"%{physxDir}/physx/source/foundation/unix/FdUnixAtomic.cpp",
			"%{physxDir}/physx/source/foundation/unix/FdUnixMutex.cpp",
			"%{physxDir}/physx/source/foundation/unix/FdUnixSList.cpp",
			"%{physxDir}/physx/source/foundation/unix/FdUnixSocket.cpp",
			"%{physxDir}/physx/source/foundation/unix/FdUnixSync.cpp",
			"%{physxDir}/physx/source/foundation/unix/FdUnixThread.cpp",
		}
		
	filter "system:windows"
		systemversion "latest"
		files {
			"%{physxDir}/physx/source/foundation/windows/FdWindowsAtomic.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsFPU.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsMutex.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsPrintString.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsSList.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsSocket.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsSync.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsThread.cpp",
			"%{physxDir}/physx/source/foundation/windows/FdWindowsTime.cpp",
		}

		defines {
			"WIN32",
			"WIN64",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS",
			"PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll",
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