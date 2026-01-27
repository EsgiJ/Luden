@echo off
setlocal enabledelayedexpansion

REM ============================================
REM  Luden Engine - Project Build Script
REM ============================================

echo.
echo ==========================================
echo     Luden Engine - Project Builder
echo ==========================================
echo.

set "PROJECT_DIR=%~dp0"
cd /d "%PROJECT_DIR%"
set "PROJECT_NAME=SpaceWar"

echo Project: %PROJECT_NAME%
echo.

set "BUILD_CONFIG=%1"
if "%BUILD_CONFIG%"=="" set "BUILD_CONFIG=Debug"

echo Build Configuration: %BUILD_CONFIG%
echo.

REM Dynamic paths
set "GAME_DLL_DIR=%PROJECT_DIR%bin\%BUILD_CONFIG%-windows-x86_64"
set "ENGINE_DIR=C:\GameProjects\Luden"
set "RUNTIME_DIR=%ENGINE_DIR%\Runtime\bin\%BUILD_CONFIG%-windows-x86_64"
set "RESOURCES_DIR=%PROJECT_DIR%Resources"
set "BUILD_DIR=%PROJECT_DIR%Build"

echo Engine Directory: %ENGINE_DIR%
echo Runtime Directory: %RUNTIME_DIR%
echo.

echo [1/6] Preparing build directory...
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
echo [OK] Build directory created
echo.

echo [2/6] Copying game module...
if not exist "%GAME_DLL_DIR%\%PROJECT_NAME%.dll" (
    echo [ERROR] Game DLL not found!
    echo Path: %GAME_DLL_DIR%\%PROJECT_NAME%.dll
    echo Please build your project first in Visual Studio!
    pause
    exit /b 1
)
copy /y "%GAME_DLL_DIR%\%PROJECT_NAME%.dll" "%BUILD_DIR%\" > nul
echo [OK] %PROJECT_NAME%.dll copied
echo.

echo [3/6] Copying Runtime files...
if not exist "%RUNTIME_DIR%\Runtime.exe" (
    echo [ERROR] Runtime.exe not found!
    echo Path: %RUNTIME_DIR%\Runtime.exe
    echo.
    echo Please ensure Runtime project is built!
    pause
    exit /b 1
)
copy /y "%RUNTIME_DIR%\Runtime.exe" "%BUILD_DIR%\" > nul
copy /y "%RUNTIME_DIR%\Engine.dll" "%BUILD_DIR%\" > nul
echo [OK] Runtime files copied
echo.

echo [4/6] Copying dependencies...
copy /y "%RUNTIME_DIR%\*.dll" "%BUILD_DIR%\" > nul 2>&1
echo [OK] Dependencies copied
echo.

echo [5/6] Copying project file...
copy /y "%PROJECT_DIR%%PROJECT_NAME%.lproject" "%BUILD_DIR%\" > nul
echo [OK] Project file copied
echo.

echo [6/6] Copying resources...
if exist "%RESOURCES_DIR%" (
    xcopy /e /i /y /q "%RESOURCES_DIR%" "%BUILD_DIR%\Resources\" > nul
    echo [OK] Resources copied
) else (
    echo [WARNING] Resources folder not found
)
echo.

echo ==========================================
echo     BUILD SUCCESSFUL!
echo ==========================================
echo Build location: %BUILD_DIR%
echo.

if "%2"=="run" (
    echo ==========================================
    echo     Launching Game
    echo ==========================================
    echo.
    cd /d "%BUILD_DIR%"
    
    Runtime.exe
    
    echo.
    echo ==========================================
    echo     Game Closed - Exit Code: %ERRORLEVEL%
    echo ==========================================
    
    if %ERRORLEVEL% neq 0 (
        echo [ERROR] Game exited with error code: %ERRORLEVEL%
        echo.
    )
    
    pause
)

if "%2" NEQ "run" (
    pause
)

exit /b 0
