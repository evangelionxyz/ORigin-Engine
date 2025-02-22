project "SPDLOG"
    location (ThirdPartyLocation)
    language "C++"
    kind "StaticLib"
    staticruntime "off"

    targetdir (ThirdPartyOutputdir)
    objdir (ThirdPartyIntOutputdir)

    files {
        "%{THIRD_PARTY_DIR}/SPDLOG/src/async.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/bundled_fmtlib_format.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/cfg.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/color_sinks.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/file_sinks.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/spdlog.cpp",
        "%{THIRD_PARTY_DIR}/SPDLOG/src/stdout_sinks.cpp",
    }

    defines {
        "SPDLOG_COMPILED_LIB"
    }

    includedirs {
        "%{THIRD_PARTY_DIR}/SPDLOG/include",
    }

    filter "system:windows"
        buildoptions {
            "/utf-8"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"