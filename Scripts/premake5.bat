@echo off
pushd %~dp0\..\Scripts
call premake\premake5.exe gmake2
popd