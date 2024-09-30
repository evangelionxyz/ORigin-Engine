add_library(ASSIMP STATIC
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Collada/ColladaExporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Collada/ColladaHelper.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Collada/ColladaLoader.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Collada/ColladaParser.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Obj/ObjExporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Obj/ObjFileImporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Obj/ObjFileMtlImporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/Obj/ObjFileParser.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/M3D/M3DExporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/M3D/M3DImporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/M3D/M3DWrapper.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF/glTFCommon.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF/glTFImporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF2/glTF2Importer.cpp

    ${THIRD_PARTY_DIR}/Assimp/code/CApi/AssimpCExport.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/CApi/CInterfaceIOWrapper.cpp

    ${THIRD_PARTY_DIR}/Assimp/code/Common/AssertHandler.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Assimp.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Base64.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/BaseImporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/BaseProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Bitmap.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Compression.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/CreateAnimMesh.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/DefaultIOStream.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/DefaultIOSystem.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/DefaultLogger.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Exceptional.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Exporter.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/IOSystem.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Importer.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/ImporterRegistry.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/PostStepRegistry.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/RemoveComments.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/SGSpatialSort.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/SceneCombiner.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/ScenePreprocessor.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/SkeletonMeshBuilder.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/SpatialSort.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/StandardShapes.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Subdivision.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/TargetAnimation.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/Version.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/VertexTriangleAdjacency.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/ZipArchiveIOSystem.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/material.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/scene.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Common/simd.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Geometry/GeometryUtils.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/Material/MaterialSystem.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ArmaturePopulate.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/CalcTangentsProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ComputeUVMappingProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ConvertToLHProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/DeboneProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/DropFaceNormalsProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/EmbedTexturesProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/FindDegenerates.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/FindInstancesProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/FindInvalidDataProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/FixNormalsStep.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/GenBoundingBoxesProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/GenFaceNormalsProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/GenVertexNormalsProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ImproveCacheLocality.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/JoinVerticesProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/LimitBoneWeightsProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/MakeVerboseFormat.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/OptimizeGraph.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/OptimizeMeshes.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/PretransformVertices.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ProcessHelper.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/RemoveRedundantMaterials.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/RemoveVCProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ScaleProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/SortByPTypeProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/SplitByBoneCountProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/SplitLargeMeshes.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/TextureTransform.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/TriangulateProcess.cpp
    ${THIRD_PARTY_DIR}/Assimp/code/PostProcessing/ValidateDataStructure.cpp
    ${THIRD_PARTY_DIR}/Assimp/contrib/unzip/ioapi.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/unzip/unzip.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/adler32.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/compress.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/crc32.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/deflate.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzclose.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzlib.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzread.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzwrite.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/infback.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/inffast.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/inflate.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/inftrees.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/trees.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/uncompr.c
    ${THIRD_PARTY_DIR}/Assimp/contrib/zlib/zutil.c
    ${THIRD_PARTY_DIR}/Assimp/include/assimp/.editorconfig
)

target_include_directories(ASSIMP PRIVATE
  ${THIRD_PARTY_DIR}/Assimp
  ${THIRD_PARTY_DIR}/Assimp/code
  ${THIRD_PARTY_DIR}/Assimp/include
  ${THIRD_PARTY_DIR}/Assimp/contrib
  ${THIRD_PARTY_DIR}/Assimp/contrib/zip
  ${THIRD_PARTY_DIR}/Assimp/contrib/zlib
  ${THIRD_PARTY_DIR}/Assimp/contrib/unzip
  ${THIRD_PARTY_DIR}/Assimp/contrib/pugixml/src
  ${THIRD_PARTY_DIR}/Assimp/contrib/utf8cpp/source
  ${THIRD_PARTY_DIR}/Assimp/contrib/rapidjson/include
  ${THIRD_PARTY_DIR}/Assimp/contrib/openddlparser/include
)

target_compile_definitions(ASSIMP PRIVATE
  RAPIDJSON_HAS_STDSTRING=1
  # ASSIMP_BUILD_NO_GLTF_IMPORTER
  ASSIMP_BUILD_NO_X_IMPORTER
  ASSIMP_BUILD_NO_FBX_IMPORTER
  ASSIMP_BUILD_NO_3DS_IMPORTER
  #ASSIMP_BUILD_NO_MD3_IMPORTER
  ASSIMP_BUILD_NO_MDL_IMPORTER
  ASSIMP_BUILD_NO_MD2_IMPORTER
  ASSIMP_BUILD_NO_PLY_IMPORTER
  ASSIMP_BUILD_NO_ASE_IMPORTER
  ASSIMP_BUILD_NO_AMF_IMPORTER
  ASSIMP_BUILD_NO_HMP_IMPORTER
  ASSIMP_BUILD_NO_SMD_IMPORTER
  ASSIMP_BUILD_NO_MDC_IMPORTER
  ASSIMP_BUILD_NO_MD5_IMPORTER
  ASSIMP_BUILD_NO_STL_IMPORTER
  ASSIMP_BUILD_NO_LWO_IMPORTER
  ASSIMP_BUILD_NO_DXF_IMPORTER
  ASSIMP_BUILD_NO_NFF_IMPORTER
  ASSIMP_BUILD_NO_RAW_IMPORTER
  ASSIMP_BUILD_NO_OFF_IMPORTER
  ASSIMP_BUILD_NO_AC_IMPORTER
  ASSIMP_BUILD_NO_BVH_IMPORTER
  ASSIMP_BUILD_NO_IRRMESH_IMPORTER
  ASSIMP_BUILD_NO_IRR_IMPORTER
  ASSIMP_BUILD_NO_Q3D_IMPORTER
  ASSIMP_BUILD_NO_B3D_IMPORTER
  ASSIMP_BUILD_NO_TERRAGEN_IMPORTER
  ASSIMP_BUILD_NO_CSM_IMPORTER
  ASSIMP_BUILD_NO_3D_IMPORTER
  ASSIMP_BUILD_NO_LWS_IMPORTER
  ASSIMP_BUILD_NO_OGRE_IMPORTER
  ASSIMP_BUILD_NO_OPENGEX_IMPORTER
  ASSIMP_BUILD_NO_MS3D_IMPORTER
  ASSIMP_BUILD_NO_COB_IMPORTER
  ASSIMP_BUILD_NO_BLEND_IMPORTER
  ASSIMP_BUILD_NO_Q3BSP_IMPORTER
  ASSIMP_BUILD_NO_NDO_IMPORTER
  ASSIMP_BUILD_NO_IFC_IMPORTER
  ASSIMP_BUILD_NO_XGL_IMPORTER
  ASSIMP_BUILD_NO_ASSBIN_IMPORTER
  ASSIMP_BUILD_NO_C4D_IMPORTER
  ASSIMP_BUILD_NO_3MF_IMPORTER
  ASSIMP_BUILD_NO_X3D_IMPORTER
  ASSIMP_BUILD_NO_MMD_IMPORTER
  ASSIMP_BUILD_NO_STEP_EXPORTER
  ASSIMP_BUILD_NO_SIB_IMPORTER
  ASSIMP_BUILD_NO_USD_IMPORTER
  ASSIMP_BUILD_NO_PBRT_IMPORTER
  _CRT_SECURE_NO_WARNINGS
)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(ASSIMP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(ASSIMP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(ASSIMP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()