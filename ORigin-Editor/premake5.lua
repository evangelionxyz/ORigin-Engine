workspace ""
architecture "x64"
configurations {
    "Debug",
    "Release",
    "Dist"
}

flags {
    "MultiProcessorCompile"
}

project""

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

ORIGIN_ENGINE = os.getenv("ORIGIN_ENGINE_PATH")
ScriptCoreLib = path.join(ORIGIN_ENGINE, "Scripts/projects/ORigin-ScriptCore.lua")
group "ORiginEngine"
    include(ScriptCoreLib)
group ""
