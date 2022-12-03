-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Solution's Settings
workspace "ORigin"
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

    mainProjectFiles = "%{wks.location}\\%{prj.name}"
    mainOutputdir = "%{wks.location}\\bin\\%{cfg.buildcfg}\\%{prj.name}"
    mainIntOutputdir = "%{wks.location}\\bin-int\\%{cfg.buildcfg}\\%{prj.name}"

    vendorProjectFiles = "%{wks.location}\\ORigin\\vendor\\projectFiles"
    vendorOutputdir = "%{prj.location}\\bin\\%{cfg.buildcfg}\\%{prj.name}"
    vendorIntOutputdir = "%{prj.location}\\bin-int\\%{cfg.buildcfg}\\%{prj.name}"

include "projects/ProjectSwitcher.lua"
include "projects/ProjectLicenseHeader.lua"
include "dependencies/Dependencies.lua"

group "Engine"
include "projects/ORigin.lua"
include "projects/ORigin-Editor.lua"
group ""

group"Programs"
include "projects/Sandbox.lua"
include "projects/BuilderTools.lua"
group ""