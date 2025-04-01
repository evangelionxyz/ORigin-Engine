-- Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
project "BuildTool"
    kind "ConsoleApp"
    language "C#"
    dotnetframework "net8.0"
    location "%{wks.location}/Engine/BuildTool"

    vsprops {
      Nullable = "enable",
      BuiltInComInteropSupport = "true",
      AllowUnsafeBlocks = "true"
    }

    nuget {

    }
    
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