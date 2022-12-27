@echo off
pushd %~dp0\..\scripts
call premake\premake5.exe vs2022
PAUSE