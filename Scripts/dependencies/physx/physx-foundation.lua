project "NvPhysXFoundation"
kind "StaticLib"
language "C++"
staticruntime "off"
location (ThirdPartyProjectFiles)
targetdir (ThirdPartyOutputdir)
objdir (ThirdPartyIntOutputdir)

	includedirs {
		"%{PhysXDir}/physx/include",
		"%{PhysXDir}/physx/source/lowlevel/api/include"
	}
	files { 
		"%{PhysXDir}/physx/source/foundation/FdAllocator.cpp",
		"%{PhysXDir}/physx/source/foundation/FdAssert.cpp",
		"%{PhysXDir}/physx/source/foundation/FdFoundation.cpp",
		"%{PhysXDir}/physx/source/foundation/FdMathUtils.cpp",
		"%{PhysXDir}/physx/source/foundation/FdString.cpp",
		"%{PhysXDir}/physx/source/foundation/FdTempAllocator.cpp",
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
		files {"%{PhysXDir}/physx/source/foundation/src/source/**.cpp"}
		includedirs { "%{PhysXDir}/physx/source/foundation/include/unix" }
		
	filter "system:Windows"
		systemversion "latest"
		files {
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsAtomic.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsFPU.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsMutex.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsPrintString.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsSList.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsSocket.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsSync.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsThread.cpp",
			"%{PhysXDir}/physx/source/foundation/windows/FdWindowsTime.cpp",
		}

		includedirs { "%{PhysXDir}/physx/source/foundation/include/windows" }


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