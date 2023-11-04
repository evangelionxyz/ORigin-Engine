project "LowLevel"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location (vendorProjectFiles)
targetdir (vendorOutputdir)
objdir (vendorIntOutputdir)

includedirs {
	"%{physxDir}/include",
	"%{physxDir}/physx/include",
	"%{physxDir}/physx/source/Common/src/windows",
	"%{physxDir}/physx/source/LowLevel/api/include",
	"%{physxDir}/physx/source/LowLevel/software/include",
	"%{physxDir}/physx/source/LowLevelDynamics/include",
	"%{physxDir}/physx/source/LowLevel/common/include/pipeline",
	"%{physxDir}/physx/source/geomutils/include",
	"%{physxDir}/physx/source/physxgpu/include",
	"%{physxDir}/physx/source/common/src",
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
	"%{physxDir}/physx/source/lowlevel/include/api/include",
	"%{physxDir}/physx/source/lowlevel/include/common/include",
	"%{physxDir}/physx/source/lowlevel/common/include/collision",
	"%{physxDir}/physx/source/lowlevel/common/include/pipeline",
	"%{physxDir}/physx/source/lowlevel/common/include/utils",
	"%{physxDir}/physx/source/lowlevel/include/software/include",
	"%{physxDir}/physx/include/lowleveldynamics/include",
}
files { 
	"%{physxDir}/physx/source/lowlevel/**.cpp",
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
	--removefiles { "%{physxDir}/physx/source/physxcooking/src/windows/**.cpp" } 
	--includedirs { "%{physxDir}/physx/source/foundation/include/unix" }
		
filter "system:windows"
	systemversion "latest"
	--removefiles { "%{physxDir}/physx/source/foundation/src/unix/**.cpp" } 
	--includedirs { "%{physxDir}/physx/source/foundation/include/windows" }


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