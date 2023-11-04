project "PhysXExtensions"
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
	"%{physxDir}/physx/source/physxmetadata/core/include",
	"%{physxDir}/physx/source/physxmetadata/extensions/include",
	"%{physxDir}/physx/source/physxextensions/src",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Xml",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary",
	"%{physxDir}/physx/source/physxextensions/src/serialization/File",
	"%{physxDir}/physx/source/pvdsdk/src",
	"%{physxDir}/physx/source/physx/src",
	"%{physxDir}/physx/source/pvd/include",
	"%{physxDir}/physx/source/fastxml/include",
	"%{physxDir}/physx/source/foundation/include",
	"%{physxDir}/physx/source/scenequery/include",
}
files { 
	"%{physxDir}/physx/source/physxextensions/**.cpp",
	"%{vendorProjectFiles}/Binaries/Intermediates/Debug/FastXml/PsFastXml.obj",
		
	"%{physxDir}/physx/source/physxmetadata/extensions/**.h",
	"%{physxDir}/physx/source/physxmetadata/extensions/**.cpp",
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
	"FastXML",
	"PhysX",
	"PhysXCommon",
	"PhysXFoundation",
	"PhysXPvdSDK",
}

filter "system:linux"
	pic "On"
	systemversion "latest"
	includedirs {
		"%{physxDir}/physx/include/foundation/unix",
	}
		
filter "system:windows"
	systemversion "latest"
	includedirs {
		"%{physxDir}/physx/include/foundation/windows",
		"%{physxDir}/physx/source/common/src/windows",
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