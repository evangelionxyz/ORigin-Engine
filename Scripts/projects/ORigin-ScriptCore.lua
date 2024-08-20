-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
if _ACTION ~= "cmake" then
project "ORigin-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.8"
    location "../../ORigin-ScriptCore"

    targetdir ("../../ORigin-Editor/Resources/ScriptCore")
    objdir ("../../ORigin-Editor/Resources/ScriptCore/Int")

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
end