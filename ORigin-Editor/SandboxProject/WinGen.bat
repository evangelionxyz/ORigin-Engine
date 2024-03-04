cd /d %~dp0
call %ORiginEngine%\Scripts\premake\premake5.exe vs2022
call MSBuild SandboxProject.sln