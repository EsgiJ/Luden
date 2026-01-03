@echo off
setlocal

set CONFIG=Debug
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo ========================================
echo   QUICK TEST MODE
echo ========================================
echo.

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    pause
    exit /b 1
)

echo Running quick tests...
echo.
"%TEST_EXE%" --quick

echo.
echo Quick tests completed!
pause