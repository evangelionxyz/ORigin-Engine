-- Copyright (c) Evangelion Manuhutu | ORigin Engine
require "cmake"
project "ASSIMP"
  location (vendorProjectFiles)
  kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (vendorOutputdir)
	objdir (vendorIntOutputdir)

  files {
    "%{wks.location}/ORigin/vendor/Assimp/include/**",
    "%{wks.location}/ORigin/vendor/Assimp/code/Common/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/M3D/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/IQM/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/CApi/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/PostProcessing/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/Material/MaterialSystem.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/Geometry/GeometryUtils.cpp",

    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/Collada/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/Ply/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/Obj/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/FBX/**.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/glTF/glTFImporter.cpp",
    "%{wks.location}/ORigin/vendor/Assimp/code/AssetLib/glTF2/glTF2Importer.cpp",

    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/adler32.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/compress.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/crc32.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/deflate.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/gzclose.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/gzlib.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/gzread.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/gzwrite.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/infback.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/inffast.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/inflate.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/inftrees.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/trees.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/uncompr.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/zlib/zutil.c",

    "%{wks.location}/ORigin/vendor/Assimp/contrib/unzip/ioapi.c",
    "%{wks.location}/ORigin/vendor/Assimp/contrib/unzip/unzip.c",
	}
	
	includedirs {
		"%{wks.location}/ORigin/vendor/assimp",
		"%{wks.location}/ORigin/vendor/Assimp/code",
		"%{wks.location}/ORigin/vendor/Assimp/include",
		"%{wks.location}/ORigin/vendor/Assimp/contrib",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/zip",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/zlib",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/unzip",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/pugixml/src",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/utf8cpp/source",
		"%{wks.location}/ORigin/vendor/Assimp/contrib/rapidjson/include",
    "%{wks.location}/ORigin/vendor/assimpcontrib/openddlparser/include",
	}

	defines {
      -- "SWIG",
      --"ASSIMP_BUILD_NO_OWN_ZLIB",
      --"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
      --"ASSIMP_BUILD_NO_FBX_IMPORTER",
      -- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
      "ASSIMP_BUILD_NO_X_IMPORTER",
      "ASSIMP_BUILD_NO_3DS_IMPORTER",
      "ASSIMP_BUILD_NO_MD3_IMPORTER",
      "ASSIMP_BUILD_NO_MDL_IMPORTER",
      "ASSIMP_BUILD_NO_MD2_IMPORTER",
      "ASSIMP_BUILD_NO_PLY_IMPORTER",
      "ASSIMP_BUILD_NO_ASE_IMPORTER",
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
      "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
      "ASSIMP_BUILD_NO_GLTF_IMPORTER",
      "ASSIMP_BUILD_NO_C4D_IMPORTER",
      "ASSIMP_BUILD_NO_3MF_IMPORTER",
      "ASSIMP_BUILD_NO_X3D_IMPORTER",
      "ASSIMP_BUILD_NO_MMD_IMPORTER",
      "ASSIMP_BUILD_NO_STEP_EXPORTER",
      "ASSIMP_BUILD_NO_SIB_IMPORTER",     
      "ASSIMP_BUILD_NO_USD_IMPORTER",
      "ASSIMP_BUILD_NO_PBRT_IMPORTER",
      -- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
      -- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
      -- "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
      -- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
      -- "ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
      -- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
      -- "ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
      -- "ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
      -- "ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
      -- "ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
      -- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
      -- "ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
      -- "ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
      -- "ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
      -- "ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
      -- "ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
      -- "ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
      -- "ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
      -- "ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
      -- "ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
      -- "ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
      -- "ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
      -- "ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
      -- "ASSIMP_BUILD_NO_DEBONE_PROCESS",
      -- "ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
      -- "ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS"
  }

    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
        }
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"