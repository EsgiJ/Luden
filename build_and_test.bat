@echo off
setlocal

set CONFIG=Debug
set SOLUTION=Engine.sln
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

echo ========================================
echo   BUILD AND TEST
echo ========================================
echo.

REM === Check if MSBuild exists ===
if not exist %MSBUILD% (
    echo [ERROR] MSBuild not found!
    echo Please adjust the path in this script.
    pause
    exit /b 1
)

REM === Check if solution exists ===
if not exist "%SOLUTION%" (
    echo [ERROR] Solution file not found!
    echo Run generate.bat first.
    pause
    exit /b 1
)

REM === Build EngineTests project ===
echo Building EngineTests (%CONFIG%)...
echo.
%MSBUILD% %SOLUTION% /t:EngineTests /p:Configuration=%CONFIG% /p:Platform=x64 /m

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Build successful! Running tests...
echo.

REM === Run tests ===
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

"%TEST_EXE%" --all

echo.
echo ========================================
echo Build and test completed!
echo ========================================
pause