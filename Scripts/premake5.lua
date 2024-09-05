-- Copyright (c) Evangelion Manuhutu | ORigin Engine
-- Solution's Settings

require "cmake"
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
    vendorProjectFiles = "%{wks.location}/ORigin/vendor/projectFiles"
    vendorOutputdir = "%{prj.location}/bin/%{cfg.buildcfg}/%{prj.name}"
    vendorIntOutputdir = "%{prj.location}/bin-int/%{cfg.buildcfg}/%{prj.name}"
    include "dependencies/dependencies.lua"

group "Engine"
    include "projects/ORigin.lua"
    include "projects/ORigin-Editor.lua"
    include "projects/ORigin-Runtime.lua"
    include "projects/ORigin-ScriptCore.lua"
    include "projects/Sandbox.lua"
group ""
