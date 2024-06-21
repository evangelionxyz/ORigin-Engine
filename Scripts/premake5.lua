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

flags{
    "MultiProcessorCompile"
}

startproject "ORigin-Editor"

vendorProjectFiles = "%{wks.location}/ORigin/vendor/projectFiles"
vendorOutputdir = "%{prj.location}/Binaries/%{cfg.buildcfg}/%{prj.name}"
vendorIntOutputdir = "%{prj.location}/Binaries/Intermediates/%{cfg.buildcfg}/%{prj.name}"
physxDir = "%{wks.location}/ORigin/vendor/PhysX"

include "projects/VSCodeTasks.lua"
include "dependencies/dependencies.lua"

group "Engine"
include "projects/ORigin.lua"
include "projects/ORigin-Editor.lua"
include "projects/ORigin-Runtime.lua"
include "projects/ORigin-ScriptCore.lua"
include "projects/Sandbox.lua"
group ""