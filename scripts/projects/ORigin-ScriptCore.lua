-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Engine Project
project "ORigin-ScriptCore"
    location (mainProjectFiles)
    language "C#"
    dotnetframework "4.8"

    targetdir ("%{wks.location}/ORigin-Editor/resources/scripts")
    objdir ("%{wks.location}/ORigin-Editor/resources/intermediates")

    files {
	  "%{wks.location}/%{prj.name}/sources/**.cs",
	  "%{wks.location}/%{prj.name}/properties/**.cs",
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