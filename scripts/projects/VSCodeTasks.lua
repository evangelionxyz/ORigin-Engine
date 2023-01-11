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
        "label": "Build_Solution",
        "type": "shell",
        "command": "MSBuild ORigin.sln",
    },
    {
        // Build Script-Core
        "label": "Build_ScriptCore",
        "type": "shell",
        "command": "MSBuild ORigin-ScriptCore\\ORigin-ScriptCore.csproj",
    },
    {
        // Build And Run Editor (default)
        "label": "Build_Run_Editor",
        "type": "shell",
        "command": "scripts\\vscode_editor_runner.bat",
        "dependsOn": "Build_Solution",
        "group": {
            "kind": "build",
            "isDefault": true
        }
    },
    {
        // Run Editor
        "label": "Run_Editor",
        "type": "shell",
        "command": "scripts\\vscode_editor_runner.bat",
    },
    {
        // Build And Run Sandbox
        "label": "Build_Run_Sandbox",
        "type": "shell",
        "command": "scripts\\vscode_sandbox_runner.bat",
        "dependsOn": "Build_Solution",
        "group": {
            "kind": "build",
        }
    },
    {
        // Run Sandbox
        "label": "Run_Sandbox",
        "type": "shell",
        "command": "scripts\\vscode_sandbox_runner.bat",
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