@echo off
setlocal
cd /d "%~dp0"

set PLATFORM=windows-x86_64

REM --- Try Release first ---
set CONFIG=Release
set TEST_EXE=Tests\bin\%CONFIG%-%PLATFORM%\EngineTests.exe

echo.
echo ==========================================
echo   PERFORMANCE BENCHMARKS
echo ==========================================
echo   Configuration: %CONFIG%
echo   Platform: %PLATFORM%
echo ==========================================
echo.

if not exist "%TEST_EXE%" (
    echo [WARNING] Release build not found!
    echo Trying Debug build...
    echo.
    set CONFIG=Debug
    set TEST_EXE=Tests\bin\%CONFIG%-%PLATFORM%\EngineTests.exe
)

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    echo.
    echo Expected path:
    echo   %TEST_EXE%
    echo.
    echo Please build EngineTests first.
    pause
    exit /b 1
)

echo Running performance benchmarks...
echo.
pause

"%TEST_EXE%" --bench

echo.
echo ==========================================
echo   Benchmarks completed!
echo ==========================================
echo.
pause
