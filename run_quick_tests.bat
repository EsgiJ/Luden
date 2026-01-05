@echo off
setlocal

set CONFIG=Debug
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo.
echo ==========================================
echo   QUICK TESTS (No Window)
echo ==========================================
echo   Configuration: %CONFIG%
echo   Platform: %PLATFORM%
echo ==========================================
echo.

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    echo Please build the project first.
    echo.
    pause
    exit /b 1
)

echo Running quick tests (EntityMemoryPool, Serialization, ResourceManager)...
echo.
"%TEST_EXE%" --quick

echo.
echo ==========================================
echo   Quick tests completed!
echo ==========================================
echo.
pause