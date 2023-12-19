-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
os.chdir("../../")
defaultArgument =
[[/*
    This is a configuration file for the SwitchStartupProject Visual Studio Extension
    See https://heptapod.host/thirteen/switchstartupproject/blob/branch/current/Configuration.md
*/
{
  "Version": 3,
  "ListAllProjects": false,

  "MultiProjectConfigurations": {
    
    "ORigin-Editor": {
      "Projects": {
        "ORigin-Editor": {
          "StartProject": true
        }
      }
    },

    "ORigin-Runtime": {
      "Projects": {
        "ORigin-Runtime": {
          "StartProject": true
        }
      }
    },

    "Sandbox": {
      "Projects": {
        "Sandbox": {
         "StartProject": true
        }
      }
    }
  }
}]]

-- checking file available
if(os.isfile("OR1.sln.startup.json"))
then
    if(io.readfile("OR1.sln.startup.json") ~= defaultArgument)
    then
        print("\nCreating new project OR1.sln.startup.json file...")
        io.writefile("OR1.sln.startup.json", defaultArgument)
    end
else
    io.writefile("OR1.sln.startup.json", defaultArgument)
end