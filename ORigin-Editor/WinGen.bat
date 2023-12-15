cd /d %~dp0
call %ORIGIN_ENGINE_PATH%\Scripts\premake\premake5.exe vs2022
call MSBuild .sln