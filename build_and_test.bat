@echo off
setlocal

set CONFIG=Debug
set SOLUTION=LudenEngine.sln
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

echo.
echo ==========================================
echo   BUILD AND TEST
echo ==========================================
echo   Configuration: %CONFIG%
echo   Solution: %SOLUTION%
echo ==========================================
echo.

REM Check if MSBuild exists
if not exist %MSBUILD% (
    echo [ERROR] MSBuild not found at:
    echo %MSBUILD%
    echo.
    echo Please update the path in this script for your Visual Studio installation.
    echo Common locations:
    echo   - VS 2022 Community: C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe
    echo   - VS 2022 Professional: C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe
    echo   - VS 2022 Enterprise: C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe
    echo.
    pause
    exit /b 1
)

REM Check if solution exists
if not exist "%SOLUTION%" (
    echo [ERROR] Solution file not found: %SOLUTION%
    echo.
    echo Please run generate.bat first to generate the solution.
    echo.
    pause
    exit /b 1
)

REM Build EngineTests project
echo Building EngineTests project...
echo.
%MSBUILD% %SOLUTION% /t:EngineTests /p:Configuration=%CONFIG% /p:Platform=x64 /m /v:minimal

if %ERRORLEVEL% neq 0 (
    echo.
    echo ==========================================
    echo   [ERROR] Build failed!
    echo ==========================================
    echo.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ==========================================
echo   Build successful!
echo ==========================================
echo.

REM Run tests
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo Running all tests...
echo.
"%TEST_EXE%" --all

echo.
echo ==========================================
echo   Build and test completed!
echo ==========================================
echo.
pause