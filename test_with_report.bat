@echo off
setlocal

set CONFIG=Debug
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe
set REPORT_DIR=TestReports
set TIMESTAMP=%date:~-4%%date:~3,2%%date:~0,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%
set REPORT_FILE=%REPORT_DIR%\TestReport_%TIMESTAMP%.txt

echo.
echo ==========================================
echo   TEST WITH REPORT GENERATION
echo ==========================================
echo.

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    pause
    exit /b 1
)

REM Create report directory if it doesn't exist
if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"

echo Running all tests...
echo Report will be saved to: %REPORT_FILE%
echo.

REM Run tests and capture output
"%TEST_EXE%" --all > "%REPORT_FILE%" 2>&1

REM Also display output to console
type "%REPORT_FILE%"

echo.
echo ==========================================
echo   Report saved to:
echo   %REPORT_FILE%
echo ==========================================
echo.
pause