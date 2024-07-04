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

	"%{physxDir}/physx/source/lowlevel/api/include",
	"%{physxDir}/physx/source/lowlevel/software/include",
	"%{physxDir}/physx/source/lowlevel/common/include/pipeline",
	"%{physxDir}/physx/source/lowlevel/common/include/utils",
	"%{physxDir}/physx/source/lowleveldynamics/include",
	"%{physxDir}/physx/source/lowlevelaabb/include",
	"%{physxDir}/physx/source/physxgpu/include",
	"%{physxDir}/physx/source/simulationcontroller/src",
	"%{physxDir}/physx/source/simulationcontroller/include",
}
files { 
	"%{physxDir}/physx/source/physxextensions/src/ExtBroadPhase.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtCollection.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtContactJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtConvexMeshExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtCpuWorkerThread.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtCustomGeometryExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtCustomSceneQuerySystem.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtD6Joint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtDefaultCpuDispatcher.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtDefaultErrorCallback.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtDefaultSimulationFilterShader.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtDefaultStreams.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtDistanceJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtExtensions.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtFixedJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtGearJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtGjkQueryExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtMetaData.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtParticleClothCooker.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtParticleExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtPrismaticJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtPvd.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtPxStringTable.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRackAndPinionJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRaycastCCD.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRemeshingExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRevoluteJoint.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRigidActorExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtRigidBodyExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSampling.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSceneQueryExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSceneQuerySystem.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSimpleFactory.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSmoothNormals.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSoftBodyExt.cpp",
	"%{physxDir}/physx/source/physxextensions/src/ExtSphericalJoint.cpp",

	"%{physxDir}/physx/source/physxextensions/src/tet/ExtBVH.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtDelaunayBoundaryInserter.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtDelaunayTetrahedralizer.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtFastWindingNumber.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtInsideTester.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtMeshSimplificator.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtOctreeTetrahedralizer.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtRemesher.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtTetSplitting.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtTetUnionFind.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtUtilities.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtVoxelTetrahedralizer.cpp",
	"%{physxDir}/physx/source/physxextensions/src/tet/ExtBVH.cpp",

	"%{physxDir}/physx/source/physxextensions/src/serialization/SnSerialization.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnSerializationContext.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/SnSerializationRegistry.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/SnSerialUtils.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Xml/SnJointRepXSerializer.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Xml/SnRepXCoreSerializer.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Xml/SnRepXUpgrader.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Xml/SnXmlSerialization.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnBinarySerialization.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnBinaryDeserialization.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_Align.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_Convert.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_Error.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_MetaData.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_Output.cpp",
	"%{physxDir}/physx/source/physxextensions/src/serialization/Binary/SnConvX_Union.cpp",
	"%{physxDir}/physx/source/physxextensions/src/omnipvd/ExtOmniPvdRegistrationData.cpp",
	"%{physxDir}/physx/source/physxextensions/src/omnipvd/OmniPvdPxExtensionsSampler.cpp",
	"%{physxDir}/physx/source/physxmetadata/core/src/PxAutoGeneratedMetaDataObjects.cpp",
	"%{physxDir}/physx/source/physxmetadata/core/src/PxMetaDataObjects.cpp",
	"%{physxDir}/physx/source/physxmetadata/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp",

	"%{physxDir}/physx/source/physx/src/NpPhysics.cpp",
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
	"PhysXCommon",
	"PhysXFoundation",
	"PhysXPvdSDK",
	"PhysX"
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