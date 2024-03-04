workspace "SandboxProject"
architecture "x64"
configurations {
    "Debug",
    "Release",
    "Dist"
}

flags {
    "MultiProcessorCompile"
}

project"SandboxProject"

kind "SharedLib"
language "C#"
dotnetframework "4.8"
location "%{wks.location}"

targetdir ("%{prj.location}/Binaries")
objdir ("%{prj.location}/Intermediates")

files {
    "%{prj.location}/Assets/**.cs",
}

links {
    "ORigin-ScriptCore"
}

filter "system:windows"
systemversion "latest"

filter "configurations:Debug"
optimize "On"
symbols "Default"

filter "configurations:Release"
optimize "On"
symbols "Default"

filter "configurations:Dist"
optimize "Full"
symbols "Off"

engine_path = os.getenv("ORiginEngine")
ScriptCoreLib = path.join(engine_path, "Scripts/projects/ORigin-ScriptCore.lua")
group "ORiginEngine"
    include(ScriptCoreLib)
group ""
