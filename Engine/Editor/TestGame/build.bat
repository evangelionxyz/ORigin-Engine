@echo off
pushd %~dp0
%ORiginEngine%\Scripts\premake\premake5.exe vs2022 > nul 2>&1
MSBuild TestGame.sln > nul 2>&1
popd
exit
