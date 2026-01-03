@echo off
setlocal

set CONFIG=Release
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo ========================================
echo   BENCHMARK MODE (Release)
echo ========================================
echo.

if not exist "%TEST_EXE%" (
    echo [WARNING] Release build not found!
    echo Trying Debug build...
    set CONFIG=Debug
    set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe
)

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    pause
    exit /b 1
)

echo Running performance benchmarks...
echo.
"%TEST_EXE%" --bench

echo.
echo Benchmarks completed!
pause