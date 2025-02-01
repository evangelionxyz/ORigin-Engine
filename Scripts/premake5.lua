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

    BUILD_DIR              = "%{wks.location}/Engine/Build"
    THIRD_PARTY_DIR        = "%{wks.location}/Engine/ThirdParty"

    outputDir              = "%{BUILD_DIR}/%{cfg.buildcfg}/Bin"
    intOutputDir           = "%{BUILD_DIR}/%{cfg.buildcfg}/Objs/%{prj.name}"

    ThirdPartyProjectFiles = "%{THIRD_PARTY_DIR}/Project Files"
    ThirdPartyOutputdir    = "%{ThirdPartyProjectFiles}/Build/%{cfg.buildcfg}/Bin"
    ThirdPartyIntOutputdir = "%{ThirdPartyProjectFiles}/Build/%{cfg.buildcfg}/Objs/%{prj.name}"
    PhysXDir               = "%{THIRD_PARTY_DIR}/nvphysx"
    
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
