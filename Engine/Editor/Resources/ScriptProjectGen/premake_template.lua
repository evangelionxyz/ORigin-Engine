workspace "{PROJECT_NAME}"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    flags { "MultiProcessorCompile" }
    startproject "{PROJECT_NAME}"
    project "{PROJECT_NAME}"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.8"
    location "%{wks.location}"

    targetdir ( "%{prj.location}/bin"                  )
    objdir    ( "%{prj.location}/bin-int"              )
    files     { "%{prj.location}/Assets/Scripts/**.cs" }
    links     { "ScriptCore" }

    filter "system:linux"
        pic "On"
            
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
script_core_lib = path.join(engine_path, "Engine/ScriptCore/premake5.lua")
group "ORiginEngine"
include(script_core_lib)
group ""