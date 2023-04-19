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
    "Sandbox": {
    "Projects": {
      "Sandbox": {
         "StartProject": true
        }
      }
    },

    "ORigin-Editor": {
      "Projects": {
        "ORigin-Editor": {
            "StartProject": true
        }
      }
    }
  }
}]]

-- checking file available
if(os.isfile("ORigin.sln.startup.json"))
then
    if(io.readfile("ORigin.sln.startup.json") ~= defaultArgument)
    then
        print("\nCreating new project ORigin.sln.startup.json file...")
        io.writefile("ORigin.sln.startup.json", defaultArgument)
    end
else
    io.writefile("ORigin.sln.startup.json", defaultArgument)
end