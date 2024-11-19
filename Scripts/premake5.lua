-- Copyright (c) Evangelion Manuhutu | ORigin Engine
-- Solution's Settings

workspace "OR1"
    location "../"
    architecture "x64"
    configurations{
        "Debug",
        "Release",
        "Dist"
    }

    flags{ "MultiProcessorCompile" }

    startproject "Editor"
    BUILD_DIR              = "%{wks.location}/Engine/Build/VS2022"
    THIRD_PARTY_DIR        = "%{wks.location}/Engine/ThirdParty"

    outputDir              = "%{BUILD_DIR}/%{cfg.buildcfg}/Binaries"
    intOutputDir           = "%{BUILD_DIR}/%{cfg.buildcfg}/Objs/%{prj.name}"

    ThirdPartyProjectFiles = "%{THIRD_PARTY_DIR}/ProjectFiles"
    ThirdPartyOutputdir    = "%{BUILD_DIR}/%{cfg.buildcfg}/Binaries/ThirdParty"
    ThirdPartyIntOutputdir = "%{BUILD_DIR}/%{cfg.buildcfg}/Objs/ThirdParty/%{prj.name}"
    
    include "dependencies/dependencies.lua"

group "Engine"
    include "../Engine/ORigin/premake5.lua"
    include "../Engine/Editor/premake5.lua"
    include "../Engine/Runtime/premake5.lua"
    include "../Engine/ScriptCore/premake5.lua"
    include "../Engine/Sandbox/premake5.lua"
group ""

group "Test"
    include "../Engine/Test/AudioTest/premake5.lua"
    include "../Engine/Test/MeshTest/premake5.lua"
group ""
