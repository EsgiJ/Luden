@echo off
setlocal

REM === Configuration ===
set CONFIG=Debug
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo ========================================
echo   LUDEN ENGINE - TEST RUNNER
echo ========================================
echo.

REM === Check if tests are built ===
if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    echo Please build the project first.
    echo.
    echo Expected location: %TEST_EXE%
    echo.
    pause
    exit /b 1
)

REM === Run tests ===
echo Running all tests...
echo.
"%TEST_EXE%" --all

echo.
echo ========================================
echo Tests completed!
echo ========================================
pause