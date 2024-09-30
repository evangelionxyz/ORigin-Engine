-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "ASSIMP"
  location (ThirdPartyProjectFiles)
  kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (ThirdPartyOutputdir)
	objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/Assimp/include/**",
        "%{THIRD_PARTY_DIR}/Assimp/code/Common/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/CApi/**.cpp",

        "%{THIRD_PARTY_DIR}/Assimp/code/Pbrt/PbrtExporter.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/PostProcessing/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/Material/MaterialSystem.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/Geometry/GeometryUtils.cpp",

        --"%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/FBX/**.cpp",
        --"%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/Ply/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/M3D/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/IQM/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/Collada/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/Obj/**.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF/glTFCommon.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF/glTFImporter.cpp",
        "%{THIRD_PARTY_DIR}/Assimp/code/AssetLib/glTF2/glTF2Importer.cpp",

        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/adler32.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/compress.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/crc32.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/deflate.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzclose.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzlib.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzread.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/gzwrite.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/infback.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/inffast.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/inflate.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/inftrees.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/trees.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/uncompr.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib/zutil.c",

        "%{THIRD_PARTY_DIR}/Assimp/contrib/unzip/ioapi.c",
        "%{THIRD_PARTY_DIR}/Assimp/contrib/unzip/unzip.c",
	}
	
	includedirs {
		  "%{THIRD_PARTY_DIR}/Assimp",
		  "%{THIRD_PARTY_DIR}/Assimp/code",
		  "%{THIRD_PARTY_DIR}/Assimp/include",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/zip",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/zlib",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/unzip",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/pugixml/src",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/utf8cpp/source",
		  "%{THIRD_PARTY_DIR}/Assimp/contrib/rapidjson/include",
      "%{THIRD_PARTY_DIR}/Assimp/contrib/openddlparser/include",
	}

	defines {
      "RAPIDJSON_HAS_STDSTRING=1",
      -- "SWIG",
      -- "ASSIMP_BUILD_NO_OWN_ZLIB",
      -- "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
      -- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
      -- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
      "ASSIMP_BUILD_NO_FBX_IMPORTER",
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
      "ASSIMP_BUILD_NO_C4D_IMPORTER",
      "ASSIMP_BUILD_NO_3MF_IMPORTER",
      "ASSIMP_BUILD_NO_X3D_IMPORTER",
      "ASSIMP_BUILD_NO_MMD_IMPORTER",
      "ASSIMP_BUILD_NO_STEP_EXPORTER",
      "ASSIMP_BUILD_NO_SIB_IMPORTER",     
      "ASSIMP_BUILD_NO_USD_IMPORTER",
      "ASSIMP_BUILD_NO_PBRT_IMPORTER",
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