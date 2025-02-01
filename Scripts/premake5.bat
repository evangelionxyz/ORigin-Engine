@echo off
pushd %~dp0\..\Scripts
call premake\premake5.exe vs2022
popd