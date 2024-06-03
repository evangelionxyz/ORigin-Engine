@echo off
msbuild OR1.sln /t:Build /p:Configuration=Release
pause