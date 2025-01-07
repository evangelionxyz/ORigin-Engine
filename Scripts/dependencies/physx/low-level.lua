project "NvLowLevel"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location (ThirdPartyProjectFiles)
targetdir (ThirdPartyOutputdir)
objdir (ThirdPartyIntOutputdir)

includedirs {
	"%{PhysXDir}/include",
	"%{PhysXDir}/physx/include",
    "%{PhysXDir}/physx/source/lowleveldynamics/shared",
	"%{PhysXDir}/physx/source/Common/src/windows",
	"%{PhysXDir}/physx/source/LowLevel/api/include",
	"%{PhysXDir}/physx/source/LowLevel/software/include",
	"%{PhysXDir}/physx/source/LowLevelDynamics/include",
	"%{PhysXDir}/physx/source/LowLevel/common/include/pipeline",
	"%{PhysXDir}/physx/source/geomutils/include",
	"%{PhysXDir}/physx/source/physxgpu/include",
	"%{PhysXDir}/physx/source/common/src",
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
	"%{PhysXDir}/physx/source/lowlevel/include/api/include",
	"%{PhysXDir}/physx/source/lowlevel/include/common/include",
	"%{PhysXDir}/physx/source/lowlevel/common/include/collision",
	"%{PhysXDir}/physx/source/lowlevel/common/include/pipeline",
	"%{PhysXDir}/physx/source/lowlevel/common/include/utils",
	"%{PhysXDir}/physx/source/lowlevel/include/software/include",
	"%{PhysXDir}/physx/include/lowleveldynamics/include",
}
files { 
	"%{PhysXDir}/physx/source/lowlevel/**.cpp",
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
filter "system:windows"
	systemversion "latest"


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