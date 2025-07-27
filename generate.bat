@echo off
REM === Generate Visual Studio 2022 Solution ===

REM Premake5 executable path (eğer PATH'te yoksa tam yol ver)
set PREMAKE_EXE=premake5.exe

echo Generating Visual Studio 2022 solution...
%PREMAKE_EXE% vs2022

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Premake5 execution failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Done.
pause