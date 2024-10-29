pushd %~dp0
premake5 vs2022
MSBuild {PROJECT_NAME}.sln
popd
exit
