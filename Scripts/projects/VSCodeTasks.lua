-- Copyright (c) Evangelion Manuhutu | ORigin Engine
os.chdir("../../")
argument =
[[// Copyright (c) Evangelion Manuhutu | ORigin Engine
// ORigin Project Specification for VSCode
{
   "version": "2.0.0",
   "tasks":[
    {
        // Build Solution
        "label": "Solution Build - Release",
        "type": "shell",
        "command": "MSBuild ORigin.sln /p:Configuration=Release",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Rebuild Solution
        "label": "Solution Rebuild - Release",
        "type": "shell",
        "command": "MSBuild ORigin.sln /t:Rebuild /p:Configuration=Release",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build Solution
        "label": "Solution Build - Debug",
        "type": "shell",
        "command": "MSBuild ORigin.sln /p:Configuration=Debug",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Rebuild Solution
        "label": "Solution Rebuild - Debug",
        "type": "shell",
        "command": "MSBuild ORigin.sln /t:Rebuild /p:Configuration=Debug",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Clean Solution
        "label": "Solution Clean",
        "type": "shell",
        "command": "MSBuild ORigin.sln /t:Clean",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build Script-Core
        "label": "Script-Core Build",
        "type": "shell",
        "command": "MSBuild ORigin-ScriptCore\\ORigin-ScriptCore.csproj",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build And Run Editor (default)
        "label": "Editor Build & Run",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "ORigin-Editor",
        },
        "dependsOn": "Solution Build - Debug",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Build Editor
        "label": "Editor Build",
        "type": "shell",
        "command": "MSBuild ORigin-Editor\\ORigin-Editor.vcxproj",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Run Editor
        "label": "Editor Run",
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
        "label": "Sandbox Build & Run",
        "type": "shell",
        "command": "vscode_runner.bat",
        "options": {
            "cwd": "Sandbox",
        },
        "dependsOn": "Solution Build - Debug",
        "group": {
            "kind": "build",
            "isDefault": true
        },
        
    },
    {
        // Run Sandbox
        "label": "Sandbox Run",
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