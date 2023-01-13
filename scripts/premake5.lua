-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Solution's Settings
workspace "ORigin"
    kind "SharedLib"
    location "../"
    architecture "x64"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

    flags {
		"MultiProcessorCompile"
	}

    startproject "ORigin-Editor"

    mainProjectFiles = "%{wks.location}%{prj.name}"
    mainOutputdir = "%{wks.location}/Binaries/%{cfg.buildcfg}/ORigin"
    mainIntOutputdir = "%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}/ORigin"

    vendorProjectFiles = "%{wks.location}/ORigin/vendor/projectFiles"
    vendorOutputdir = "%{prj.location}/Binaries/%{cfg.buildcfg}/%{prj.name}"
    vendorIntOutputdir = "%{prj.location}/Binaries/Intermediates/%{cfg.buildcfg}/%{prj.name}"

include "projects/ProjectSwitcher.lua"
include "projects/ProjectLicenseHeader.lua"
include "projects/VSCodeTasks.lua"
include "dependencies/Dependencies.lua"

group "Engine"
include "projects/ORigin.lua"
include "projects/ORigin-Editor.lua"
include "projects/ORigin-ScriptCore.lua"
include "projects/Sandbox.lua"
group ""