-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
os.chdir("../../")
argument =
[[// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
// ORigin Project Specification for VSCode
{
   "version": "2.0.0",
   "tasks":[
    {
        // Build Solution
        "label": "Build Solution",
        "type": "shell",
        "command": "MSBuild ORigin.sln",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build Script-Core
        "label": "Build Script Core",
        "type": "shell",
        "command": "MSBuild ORigin-ScriptCore\\ORigin-ScriptCore.csproj",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build And Run Editor (default)
        "label": "Build & Run Editor",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "ORigin-Editor",
        },
        "dependsOn": "Build_Solution",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Run Editor
        "label": "Run Editor",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "ORigin-Editor",
        },
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build And Run Sandbox
        "label": "Build & Run Sandbox",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "Sandbox",
        },
        "dependsOn": "Build_Solution",
        "group": {
            "kind": "build",
            "isDefault": true
        },
        
    },
    {
        // Run Sandbox
        "label": "Run Sandbox",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "Sandbox",
        },
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
]
}]]

-- checking file available
if(os.isfile(".vscode/tasks.json"))
then
    if(io.readfile("tasks.json") ~= argument)
    then
        print("\nCreating new project tasks.json file...")
        io.writefile(".vscode/tasks.json", argument)
    end
else
    io.writefile(".vscode/tasks.json", argument)
end