project "NvPhysXExtensions"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location (ThirdPartyProjectFiles)
targetdir (ThirdPartyOutputdir)
objdir (ThirdPartyIntOutputdir)

includedirs {
	"%{PhysXDir}/physx/include",
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
	"%{PhysXDir}/physx/source/physxmetadata/core/include",
	"%{PhysXDir}/physx/source/physxmetadata/extensions/include",
	"%{PhysXDir}/physx/source/physxextensions/src",
	"%{PhysXDir}/physx/source/physxextensions/src/serialization/Xml",
	"%{PhysXDir}/physx/source/physxextensions/src/serialization/Binary",
	"%{PhysXDir}/physx/source/physxextensions/src/serialization/File",
	"%{PhysXDir}/physx/source/pvdsdk/src",
	"%{PhysXDir}/physx/source/physx/src",
	"%{PhysXDir}/physx/source/pvd/include",
	"%{PhysXDir}/physx/source/fastxml/include",
	"%{PhysXDir}/physx/source/foundation/include",
	"%{PhysXDir}/physx/source/scenequery/include",
}
files { 
	"%{PhysXDir}/physx/source/physxextensions/**.cpp",
	"%{vendorProjectFiles}/Binaries/Intermediates/Debug/FastXml/PsFastXml.obj",
		
	"%{PhysXDir}/physx/source/physxmetadata/extensions/**.h",
	"%{PhysXDir}/physx/source/physxmetadata/extensions/**.cpp",
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

links {
	"NvPhysX",
	"NvPhysXCommon",
	"NvPhysXFoundation",
	"NvPhysXPvdSDK",
}

filter "system:linux"
	pic "On"
	systemversion "latest"
	includedirs {
		"%{PhysXDir}/physx/include/foundation/unix",
	}
		
filter "system:windows"
	systemversion "latest"
	includedirs {
		"%{PhysXDir}/physx/include/foundation/windows",
		"%{PhysXDir}/physx/source/common/src/windows",
	}


filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
	defines {
		"PX_DEBUG=1",
		"PX_CHECKED=1",
		"PX_NVTX=0",
		"_DEBUG"
	}

filter "configurations:Release"
	runtime "Release"
	optimize "On"
	symbols "On"
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