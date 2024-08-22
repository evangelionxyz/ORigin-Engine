-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

project "ORigin-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.8"
    location "%{wks.location}/ORigin-ScriptCore"

    targetdir ("%{wks.location}/ORigin-Editor/Resources/ScriptCore")
    objdir ("%{wks.location}/ORigin-Editor/Resources/ScriptCore/Int")

    files {
        "%{prj.location}/sources/**.cs",
        "%{prj.location}/properties/**.cs",
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