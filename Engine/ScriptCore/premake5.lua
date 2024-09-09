-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
project "ORigin-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.8"
    location "%{wks.location}/Engine/ScriptCore"
    
    targetdir (outputDir)
    objdir (intOutputDir)

    files {
        "sources/**.cs",
        "properties/**.cs",
    }

    filter "configurations:Debug"
        optimize "On"
        symbols "Default"

    filter "configurations:Release"
        optimize "Full"
        symbols "Default"

    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"