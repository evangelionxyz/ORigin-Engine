-- Copyright (c) Evangelion Manuhutu | ORigin Engine

project "ASSIMP"
  location (vendorProjectFiles)
  kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

  files {
    "%{wks.location}/ORigin/vendor/assimp/include/**",
    "%{wks.location}/ORigin/vendor/assimp/code/Common/**.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/M3D/**.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/IQM/**.cpp",

    "%{wks.location}/ORigin/vendor/assimp/code/CApi/**.cpp",

    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Collada/ColladaLoader.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Collada/ColladaParser.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Ply/PlyParser.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Ply/PlyLoader.cpp",

    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/EmbedTexturesProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ConvertToLHProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/GenVertexNormalsProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ArmaturePopulate.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/GenBoundingBoxesProcess.cpp",

    "%{wks.location}/ORigin/vendor/assimp/code/Material/MaterialSystem.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ProcessHelper.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ScaleProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/TriangulateProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ValidateDataStructure.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Obj/ObjFileImporter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Obj/ObjFileMtlImporter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXAnimation.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXBinaryTokenizer.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXConverter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXDeformer.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXDocument.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXDocumentUtil.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXImporter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXMaterial.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXMeshGeometry.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXModel.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXNodeAttribute.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXParser.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXProperties.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXTokenizer.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXExporter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXExportNode.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXExportProperty.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/FBX/FBXUtil.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/Obj/ObjFileParser.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/glTF/glTFImporter.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/AssetLib/glTF2/glTF2Importer.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/MakeVerboseFormat.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/CalcTangentsProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/ScaleProcess.cpp",
    "%{wks.location}/ORigin/vendor/assimp/code/PostProcessing/EmbedTexturesProcess.cpp",

    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/adler32.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/compress.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/crc32.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/deflate.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/gzclose.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/gzlib.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/gzread.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/gzwrite.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/infback.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/inffast.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/inflate.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/inftrees.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/trees.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/uncompr.c",
    "%{wks.location}/ORigin/vendor/assimp/contrib/zlib/zutil.c",
	}
	
	includedirs {
		"%{wks.location}/ORigin/vendor/assimp",
		"%{wks.location}/ORigin/vendor/assimp/code",
		"%{wks.location}/ORigin/vendor/assimp/include",
		"%{wks.location}/ORigin/vendor/assimp/contrib",
		"%{wks.location}/ORigin/vendor/assimp/contrib/zip",
		"%{wks.location}/ORigin/vendor/assimp/contrib/zlib",
		"%{wks.location}/ORigin/vendor/assimp/contrib/unzip",
		"%{wks.location}/ORigin/vendor/assimp/contrib/pugixml/src",
		"%{wks.location}/ORigin/vendor/assimp/contrib/utf8cpp/source",
		"%{wks.location}/ORigin/vendor/assimp/contrib/rapidjson/include",
    "%{wks.location}/ORigin/vendor/assimpcontrib/openddlparser/include",
	}

	defines {
      -- "SWIG",
      --"ASSIMP_BUILD_NO_OWN_ZLIB",
      "ASSIMP_BUILD_NO_X_IMPORTER",
      "ASSIMP_BUILD_NO_3DS_IMPORTER",
      "ASSIMP_BUILD_NO_MD3_IMPORTER",
      "ASSIMP_BUILD_NO_MDL_IMPORTER",
      "ASSIMP_BUILD_NO_MD2_IMPORTER",
      "ASSIMP_BUILD_NO_PLY_IMPORTER",
      "ASSIMP_BUILD_NO_ASE_IMPORTER",
      -- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
      "ASSIMP_BUILD_NO_AMF_IMPORTER",
      "ASSIMP_BUILD_NO_HMP_IMPORTER",
      "ASSIMP_BUILD_NO_SMD_IMPORTER",
      "ASSIMP_BUILD_NO_MDC_IMPORTER",
      "ASSIMP_BUILD_NO_MD5_IMPORTER",
      "ASSIMP_BUILD_NO_STL_IMPORTER",
      "ASSIMP_BUILD_NO_LWO_IMPORTER",
      "ASSIMP_BUILD_NO_DXF_IMPORTER",
      "ASSIMP_BUILD_NO_NFF_IMPORTER",
      "ASSIMP_BUILD_NO_RAW_IMPORTER",
      "ASSIMP_BUILD_NO_OFF_IMPORTER",
      "ASSIMP_BUILD_NO_AC_IMPORTER",
      "ASSIMP_BUILD_NO_BVH_IMPORTER",
      "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
      "ASSIMP_BUILD_NO_IRR_IMPORTER",
      "ASSIMP_BUILD_NO_Q3D_IMPORTER",
      "ASSIMP_BUILD_NO_B3D_IMPORTER",
      "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
      "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
      "ASSIMP_BUILD_NO_CSM_IMPORTER",
      "ASSIMP_BUILD_NO_3D_IMPORTER",
      "ASSIMP_BUILD_NO_LWS_IMPORTER",
      "ASSIMP_BUILD_NO_OGRE_IMPORTER",
      "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
      "ASSIMP_BUILD_NO_MS3D_IMPORTER",
      "ASSIMP_BUILD_NO_COB_IMPORTER",
      "ASSIMP_BUILD_NO_BLEND_IMPORTER",
      "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
      "ASSIMP_BUILD_NO_NDO_IMPORTER",
      "ASSIMP_BUILD_NO_IFC_IMPORTER",
      "ASSIMP_BUILD_NO_XGL_IMPORTER",
      --"ASSIMP_BUILD_NO_FBX_IMPORTER",
      "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
      "ASSIMP_BUILD_NO_GLTF_IMPORTER",
      "ASSIMP_BUILD_NO_C4D_IMPORTER",
      "ASSIMP_BUILD_NO_3MF_IMPORTER",
      "ASSIMP_BUILD_NO_X3D_IMPORTER",
      "ASSIMP_BUILD_NO_MMD_IMPORTER",
      "ASSIMP_BUILD_NO_STEP_EXPORTER",
      "ASSIMP_BUILD_NO_SIB_IMPORTER",     
      -- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
      -- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
      "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
      -- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
      "ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
      -- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",/home/evangelion/Dev/ORigin-Engine/ORigin/vendor/assimp/CMakeLists.txt
      "ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
      "ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
      "ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
      "ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
      -- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
      "ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
      "ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
      "ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
      "ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
      "ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
      "ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
      "ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
      "ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
      "ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
      "ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
      "ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
      "ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
      "ASSIMP_BUILD_NO_DEBONE_PROCESS",
      "ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
      "ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS"
  }

  filter "system:linux"
        pic "On"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"