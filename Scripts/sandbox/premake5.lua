workspace "ORiginSandbox"
location "../../ORigin-Editor/SandboxProject"
architecture "x64"

configurations {
    "Debug",
    "Release",
    "Dist"
}

flags {
    "MultiProcessorCompile"
}

startproject "Sandbox"

project "Sandbox"
kind "SharedLib"
language "C#"
dotnetframework "4.8"
location "%{wks.location}"

targetdir ("%{prj.location}/Binaries")
objdir ("%{prj.location}/Intermediates")

files
{
    "%{prj.location}/sources/**.cs",
    "%{prj.location}/properties/**.cs",
}

links
{
    "ORigin-ScriptCore"
}

filter "system:windows"
    systemversion "latest"
    postbuildcommands
    {
        "{COPYDIR} %{prj.location}Binaries ../../../Binaries/%{cfg.buildcfg}/ORigin/SandboxProject/Binaries",
    }

filter "configurations:Debug"
optimize "On"
symbols "Default"

filter "configurations:Release"
optimize "On"
symbols "Default"

filter "configurations:Dist"
optimize "Full"
symbols "Off"

group "ORigin"
    include "../projects/ORigin-ScriptCore.lua"
group ""