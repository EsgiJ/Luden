@echo off
setlocal
cd /d "%~dp0"

set PLATFORM=windows-x86_64

REM Check for configuration parameter
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug

REM Validate configuration
if /i not "%CONFIG%"=="Debug" if /i not "%CONFIG%"=="Release" (
    echo [ERROR] Invalid configuration: %CONFIG%
    echo Usage: run_tests.bat [Debug^|Release]
    echo.
    pause
    exit /b 1
)

set TEST_EXE=Tests\bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo.
echo ==========================================
echo   LUDEN ENGINE - TEST RUNNER
echo ==========================================
echo   Configuration: %CONFIG%
echo   Platform: %PLATFORM%
echo ==========================================
echo.

if not exist "%TEST_EXE%" (
    echo [ERROR] %CONFIG% build not found!
    echo Expected: %TEST_EXE%
    echo.
    echo Please build the project in %CONFIG% mode first.
    echo.
    pause
    exit /b 1
)

echo Starting all tests...
echo.

"%TEST_EXE%" --all

set TEST_EXIT_CODE=%ERRORLEVEL%

echo.
echo ==========================================
if %TEST_EXIT_CODE% equ 0 (
    echo   Tests completed successfully!
) else (
    echo   Tests completed with errors!
    echo   Exit Code: %TEST_EXIT_CODE%
)
echo ==========================================
echo.
pause
exit /b %TEST_EXIT_CODE%