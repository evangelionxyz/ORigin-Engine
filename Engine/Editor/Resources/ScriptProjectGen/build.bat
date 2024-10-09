@echo off
pushd %~dp0
premake5 vs2022 > nul 2>&1
MSBuild {PROJECT_NAME}.sln > nul 2>&1
popd
exit
