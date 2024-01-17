@echo off
net session >nul 2>&1
if %errorlevel% neq 0 (
    powershell -Command Start-Process -FilePath "%0" -Verb RunAs
    exit /b
)

pushd %~dp0\Scripts
python Setup.py
pause
