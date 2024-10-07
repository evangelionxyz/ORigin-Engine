workspace "TestGame"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    flags { "MultiProcessorCompile" }
    startproject "TestGame"
    project "TestGame"
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
