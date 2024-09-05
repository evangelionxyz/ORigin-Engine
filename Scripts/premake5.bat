@echo off
setlocal

:menu
echo Choose the build system:
echo [1] Visual Studio 2022
echo [2] GNU Make (gmake2)
set /p choice="Enter your choice: "

if %choice%==1 (
    set build_system=vs2022
) else if %choice%==2 (
    set build_system=gmake2
) else (
    echo Invalid choice. Please try again.
    goto menu
)

pushd %~dp0\..\Scripts
call premake\premake5.exe %build_system%
popd

endlocal