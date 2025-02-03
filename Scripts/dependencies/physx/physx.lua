project "NvPhysX"
kind "StaticLib"
language "C++"
cppdialect "C++17"
staticruntime "off"
location (ThirdPartyProjectFiles)
targetdir (ThirdPartyOutputdir)
objdir (ThirdPartyIntOutputdir)

includedirs {
	"%{PhysXDir}/physx/include",
    "%{PhysXDir}/physx/source/lowleveldynamics/shared",
	"%{PhysXDir}/physx/source/common/include",
	"%{PhysXDir}/physx/source/common/src",
	"%{PhysXDir}/physx/source/physx/src",
	"%{PhysXDir}/physx/source/physx/src/device",
	"%{PhysXDir}/physx/source/physx/src/buffering",
	"%{PhysXDir}/physx/source/physxgpu/include",
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
	"%{PhysXDir}/physx/source/geomutils/include",
	"%{PhysXDir}/physx/source/lowlevel/api/include",
	"%{PhysXDir}/physx/source/lowlevel/common/include/utils",
	"%{PhysXDir}/physx/source/lowlevel/software/include",
	"%{PhysXDir}/physx/source/lowlevel/common/include/pipeline",
	"%{PhysXDir}/physx/source/lowlevelaabb/include",
	"%{PhysXDir}/physx/source/lowleveldynamics/include",
	"%{PhysXDir}/physx/source/simulationcontroller/include",
	"%{PhysXDir}/physx/source/simulationcontroller/src",
	"%{PhysXDir}/physx/source/physxcooking/src",
	"%{PhysXDir}/physx/source/physxcooking/src/mesh",
	"%{PhysXDir}/physx/source/physxcooking/src/convex",
	"%{PhysXDir}/physx/source/scenequery/include",
	"%{PhysXDir}/physx/source/physxmetadata/core/include",
	"%{PhysXDir}/physx/source/immediatemode/include",
	"%{PhysXDir}/physx/source/pvd/include",
}
links {
	"kernel32.lib",
	"user32.lib",
	"gdi32.lib",
	"winspool.lib",
	"shell32.lib",
	"ole32.lib",
	"oleaut32.lib",
	"uuid.lib",
	"comdlg32.lib",
	"advapi32.lib",
		
	"NvLowLevel",
	"NvLowLevelAABB",
	"NvLowLevelDynamics",
	"NvPhysXCommon",
	"NvPhysXFoundation",
	"NvPhysXPvdSDK",
	"NvPhysXTask",
	"NvSceneQuery",
	"NvSimulationController"
}
files {
	"%{PhysXDir}/physx/source/physx/src/gpu/PxGpu.cpp",
	"%{PhysXDir}/physx/source/physx/src/gpu/PxPhysXGpuModuleLoader.cpp",
	"%{PhysXDir}/physx/source/physx/src/omnipvd/**.cpp",

	"%{PhysXDir}/physx/source/physx/src/NpActor.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpAggregate.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpArticulationJointReducedCoordinate.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpArticulationLink.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpArticulationMimicJoint.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpArticulationReducedCoordinate.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpArticulationTendon.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpBounds.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpCheck.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpConstraint.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpDebugViz.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpDirectGPUAPI.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpFactory.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpFEMCloth.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpFEMClothMaterial.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpFEMSoftBodyMaterial.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpHairSystem.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpMaterial.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpMetaData.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpParticleBuffer.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpPBDMaterial.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpPhysics.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpPruningStructure.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpPvdSceneClient.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpPvdSceneQueryCollector.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpRigidDynamic.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpRigidStatic.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpScene.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpSceneFetchResults.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpSceneQueries.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpSerializerAdapter.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpShape.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpShapeManager.cpp",
	"%{PhysXDir}/physx/source/physx/src/NpSoftBody.cpp",
	"%{PhysXDir}/physx/source/physx/src/PvdMetaDataPvdBinding.cpp",
	"%{PhysXDir}/physx/source/physx/src/PvdPhysicsClient.cpp",

	"%{PhysXDir}/physx/source/immediatemode/src/**.cpp",
	"%{PhysXDir}/physx/source/physxmetadata/core/**.cpp",
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
	files {
		"%{PhysXDir}/physx/source/physx/src/device/linux/PhysXIndicatorLinux.cpp",
	}
    
filter "system:windows"
	systemversion "latest"
	characterset "MBCS" -- Multi-Byte Character Set
	files {
		"%{PhysXDir}/physx/source/physx/src/device/windows/PhysXIndicatorWindows.cpp",
		"%{PhysXDir}/physx/source/physx/src/windows/NpWindowsDelayLoadHook.cpp",
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