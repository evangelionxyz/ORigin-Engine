@echo off
pushd %~dp0\..\..\..\..\Scripts\sandbox
call ..\premake\premake5.exe vs2022

pushd %~dp0\..\..\..\..\ORigin-Editor\SandboxProject
call MSBuild ORiginSandbox.sln