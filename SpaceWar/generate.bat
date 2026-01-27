@echo off
REM === Generate Visual Studio 2022 Solution ===
echo Generating Visual Studio 2022 solution for project...

REM Set LUDEN_ENGINE_PATH for this session
set LUDEN_ENGINE_PATH=C:\GameProjects\Luden

echo Using Engine Path: %LUDEN_ENGINE_PATH%

REM Run premake from project directory
premake5.exe vs2022

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Premake5 execution failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Done! Solution generated successfully.
pause
